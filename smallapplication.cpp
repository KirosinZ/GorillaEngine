#include "smallapplication.hpp"

#include <iostream>
#include <string>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

#include <fstream>

#include <glm/gtx/transform.hpp>

#include <../geom.hpp>


namespace gorilla
{

small_application::small_application(GLFWwindow* wnd, std::shared_ptr<vkr::environment> env, std::shared_ptr<vkr::swapchain> display)
	: wnd_(wnd),
	  env_(env),
	  display_(display),
	  total_frames(display->image_views().size())
{
	init();
}

void small_application::init()
{
	vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, env_->graphics_family());

	command_pool_ = env_->device().createCommandPool(command_pool_create_info);

	std::vector<vk::Format> depth_format_candidates = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD24UnormS8Uint
	};
	vk::Format depth_format;
	for (vk::Format fmt : depth_format_candidates)
	{
		vk::FormatProperties props = env_->phys_device().getFormatProperties(fmt);
		if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			depth_format = fmt;
			break;
		}
	}

	uint32_t gf = env_->graphics_family();
	vk::ImageCreateInfo depth_image_create_info(
		{ },
		vk::ImageType::e2D,
		depth_format,
		vk::Extent3D{ display_->image_extent().width, display_->image_extent().height, 1 },
		1,
		1,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::SharingMode::eExclusive,
		gf,
		vk::ImageLayout::eUndefined);
	depth_image_ = env_->device().createImage(depth_image_create_info);


	vk::ImageMemoryRequirementsInfo2 depth_image_mem_reqs_info(*depth_image_);
	vk::MemoryRequirements2 depth_mem_reqs = env_->device().getImageMemoryRequirements2(depth_image_mem_reqs_info);
	int mem_type_index = -1;
	for (int mti = 0; mti < env_->phys_device().getMemoryProperties().memoryTypeCount; ++mti)
	{
		vk::MemoryType type = env_->phys_device().getMemoryProperties().memoryTypes[mti];
		if (depth_mem_reqs.memoryRequirements.memoryTypeBits & (1 << mti)
		    && (type.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		       == vk::MemoryPropertyFlagBits::eDeviceLocal)
		{
			mem_type_index = mti;
			break;
		}
	}
	vk::MemoryAllocateInfo depth_mem_alloc_info(
		depth_mem_reqs.memoryRequirements.size,
		mem_type_index);
	depth_image_memory_ = env_->device().allocateMemory(depth_mem_alloc_info);

	vk::BindImageMemoryInfo bind_image_memory_info(*depth_image_, *depth_image_memory_, 0);
	env_->device().bindImageMemory2(bind_image_memory_info);

	vk::ImageViewCreateInfo depth_view_create_info(
		{ },
		*depth_image_,
		vk::ImageViewType::e2D,
		depth_format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			vk::ImageAspectFlagBits::eDepth,
			0,
			1,
			0,
			1));
	depth_view_ = env_->device().createImageView(depth_view_create_info);


	vk::CommandBufferAllocateInfo command_buffer_allocate_info(
		*command_pool_,
		vk::CommandBufferLevel::ePrimary,
		total_frames);
	cmds_ = env_->device().allocateCommandBuffers(command_buffer_allocate_info);

	// render_pass
	vk::AttachmentDescription color_attachment(
		{ },
		display_->image_format(),
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentDescription depth_attachment(
		{ },
		depth_format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference color_ref(
		0,
		vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentReference depth_ref(
		1,
		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription ui_pass(
		{ },
		vk::PipelineBindPoint::eGraphics,
		{ },
		color_ref,
		{ },
		&depth_ref,
		{ });

	std::vector<vk::AttachmentDescription> attachments = {
		color_attachment,
		depth_attachment,
	};

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		{ },
		vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	vk::RenderPassCreateInfo render_pass_create_info(
		{ },
		attachments,
		ui_pass,
		dependency);
	render_pass_ = env_->device().createRenderPass(render_pass_create_info);

	for (int i = 0; i < display_->image_views().size(); i++)
	{
		std::vector<vk::ImageView> attachments = {
			*display_->image_views()[i],
			*depth_view_,
		};
		vk::FramebufferCreateInfo framebuffer_create_info(
			{ },
			*render_pass_,
			attachments,
			display_->image_extent().width,
			display_->image_extent().height,
			1);
		framebuffers_.push_back(env_->device().createFramebuffer(framebuffer_create_info));
	}

	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	frame_fences.reserve(total_frames);

	vk::SemaphoreCreateInfo semaphore_create_info{ };
	image_acquisition_semaphores.reserve(total_frames);
	image_presentation_semaphores.reserve(total_frames);
	for (int i = 0; i < total_frames; i++)
	{
		frame_fences.push_back(env_->device().createFence(fence_create_info));
		image_acquisition_semaphores.push_back(env_->device().createSemaphore(semaphore_create_info));
		image_presentation_semaphores.push_back(env_->device().createSemaphore(semaphore_create_info));
	}

	command_buffer_allocate_info = vk::CommandBufferAllocateInfo(
		*command_pool_,
		vk::CommandBufferLevel::ePrimary,
		1);
	vk::raii::CommandBuffer tmp_cmd = std::move(env_->device().allocateCommandBuffers(command_buffer_allocate_info)[0]);

	std::vector<vk::DescriptorPoolSize> sizes = {
		{ vk::DescriptorType::eUniformBuffer,        1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
	};

	vk::DescriptorPoolCreateInfo descriptor_pool_create_info(
		{ },
		10,
		sizes);
	imgui_descriptor_pool_ = env_->device().createDescriptorPool(descriptor_pool_create_info);

	ImGui_ImplVulkan_InitInfo init_info(
		*env_->instance(),
		*env_->phys_device(),
		*env_->device(),
		env_->graphics_family(),
		*env_->graphics_queue(),
		VK_NULL_HANDLE,
		*imgui_descriptor_pool_,
		0,
		display_->image_views().size(),
		display_->image_views().size(),
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		nullptr,
		nullptr);

	imgui_context_ = std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)>(ImGui::CreateContext(), &ImGui::DestroyContext);
	ImGui_ImplGlfw_InitForVulkan(wnd_, true);
	ImGui_ImplVulkan_Init(&init_info, *render_pass_);

	vk::CommandBufferBeginInfo command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	tmp_cmd.begin(command_buffer_begin_info);

	ImGui_ImplVulkan_CreateFontsTexture(*tmp_cmd);

	tmp_cmd.end();
	vk::SubmitInfo submit_info(
		{ },
		{ },
		*tmp_cmd);
	env_->graphics_queue().submit(submit_info);
	env_->graphics_queue().waitIdle();

	std::string err_msg;
	vertex_shader_ = vkr::shader::from_file(*env_, "../shader.vert.spv", err_msg);
	fragment_shader_ = vkr::shader::from_file(*env_, "../shader.frag.spv", err_msg);
	if (!err_msg.empty())
	{
		std::cerr << err_msg << std::endl;
	}

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages = {
		vertex_shader_.stage_create_info(),
		fragment_shader_.stage_create_info(),
	};

	geom geom = geom::parse("../objs/golira.obj", err_msg);
	std::vector<glm::vec3> colors(geom.n_vertices(), glm::vec3(1.0f));

	std::vector<uint32_t> indices(geom.polygon_indices().size());
	std::ranges::copy(geom.polygon_indices(), indices.begin());

	aos_mesh_primitive_ = vkr::aos_mesh_primitive(
		*env_,
		geom.vertices(),
		geom.normals(),
		colors,
		indices);

	const auto bindings = aos_mesh_primitive_.binding_descriptions();
	const auto attributes = aos_mesh_primitive_.attribute_descriptions();
	vk::PipelineVertexInputStateCreateInfo vertex_input_info(
		{ },
		bindings,
		attributes);

	const auto [topology, restart_enable] = aos_mesh_primitive_.topology();
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
		{ },
		topology,
		restart_enable);

	vk::PipelineTessellationStateCreateInfo tessellation_info{ };

	vk::Viewport vp(0, 0, display_->image_extent().width, display_->image_extent().height, 0.0f, 1.0f);
	vk::Rect2D sc({ 0, 0 }, display_->image_extent());
	vk::PipelineViewportStateCreateInfo viewport_state_info(
		{ },
		{ },
		{ });

	vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info(
		{ },
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eNone,
		vk::FrontFace::eCounterClockwise,
		false,
		0.0f,
		0.0f,
		0.0f,
		1.0f);

	vk::PipelineMultisampleStateCreateInfo multisample_state_create_info(
		{ },
		vk::SampleCountFlagBits::e1,
		false);

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info(
		{ },
		true,
		true,
		vk::CompareOp::eLess,
		false,
		false);

	vk::PipelineColorBlendAttachmentState color_blend_attachment_state(
		false);
	color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR
	                                              | vk::ColorComponentFlagBits::eG
	                                              | vk::ColorComponentFlagBits::eB
	                                              | vk::ColorComponentFlagBits::eA;
	vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info(
		{ },
		false,
		vk::LogicOp::eCopy,
		color_blend_attachment_state,
		{ 0.0f, 0.0f, 0.0f, 0.0f });

	std::vector<vk::DynamicState> dynamic_states{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
	};
	vk::PipelineDynamicStateCreateInfo dynamic_state_create_info(
		{ },
		dynamic_states);

	vk::DescriptorSetLayoutBinding descriptor_set_layout_binding(
		0,
		vk::DescriptorType::eUniformBuffer,
		1,
		vk::ShaderStageFlagBits::eVertex);
	vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(
		{ },
		descriptor_set_layout_binding);
	descriptor_set_layout_ = env_->device().createDescriptorSetLayout(descriptor_set_layout_create_info);

	vk::PipelineLayoutCreateInfo pipeline_layout_create_info(
		{ },
		*descriptor_set_layout_);
	pipeline_layout_ = env_->device().createPipelineLayout(pipeline_layout_create_info);

	vk::GraphicsPipelineCreateInfo pipeline_create_info(
		{ },
		shader_stages,
		&vertex_input_info,
		&input_assembly_info,
		&tessellation_info,
		&viewport_state_info,
		&rasterization_state_create_info,
		&multisample_state_create_info,
		&depth_stencil_state_create_info,
		&color_blend_state_create_info,
		&dynamic_state_create_info,
		*pipeline_layout_,
		*render_pass_,
		0);
	pipeline_ = std::move(env_->device().createGraphicsPipelines(nullptr, pipeline_create_info)[0]);

	std::vector<uint32_t> gfs = {
		gf,
	};

	struct UniformBuffer
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		glm::vec3 camPos = glm::vec3(0.0f, 0.0f, -1.0f);
	};

	UniformBuffer uniform_buffer{ };
	uniform_buffer.view = glm::lookAt(uniform_buffer.camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	uniform_buffer.projection = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 15.0f);

	vk::BufferCreateInfo uniform_buffer_create_info(
		{ },
		sizeof(UniformBuffer),
		vk::BufferUsageFlagBits::eUniformBuffer,
		vk::SharingMode::eExclusive,
		gfs);
	uniform_buffer_ = env_->device().createBuffer(uniform_buffer_create_info);
	uniform_buffer_memory_ = env_->memory_manager()
	                             .allocate(
		                             uniform_buffer_, vk::MemoryPropertyFlagBits::eHostVisible
		                                              | vk::MemoryPropertyFlagBits::eHostCoherent);

	std::vector<vk::BindBufferMemoryInfo> bind_infos = {
		vk::BindBufferMemoryInfo{
			*uniform_buffer_,
			*uniform_buffer_memory_,
			0
		}
	};
	env_->device().bindBufferMemory2(bind_infos);

	void* data = uniform_buffer_memory_.mapMemory(0, sizeof(UniformBuffer));
	std::memcpy(data, &uniform_buffer, sizeof(UniformBuffer));
	uniform_buffer_memory_.unmapMemory();

	vk::DescriptorPoolSize pool_size(vk::DescriptorType::eUniformBuffer, 1);
	descriptor_pool_create_info = vk::DescriptorPoolCreateInfo(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		1,
		pool_size);
	descriptor_pool_ = env_->device().createDescriptorPool(descriptor_pool_create_info);

	vk::DescriptorSetAllocateInfo descriptor_set_allocate_info(
		*descriptor_pool_,
		*descriptor_set_layout_);
	descriptor_set_ = std::move(env_->device().allocateDescriptorSets(descriptor_set_allocate_info)[0]);

	vk::DescriptorBufferInfo buffer_info(*uniform_buffer_, 0, sizeof(UniformBuffer));
	vk::WriteDescriptorSet write_descriptor_set(
		*descriptor_set_,
		0,
		0,
		vk::DescriptorType::eUniformBuffer,
		{ },
		buffer_info,
		{ });
	env_->device().updateDescriptorSets(write_descriptor_set, { });
}


void small_application::process_inputs()
{
	if (glfwGetKey(wnd_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(wnd_, true);
	}
}

void small_application::run()
{
	while (!glfwWindowShouldClose(wnd_))
	{
		glfwPollEvents();

		process_inputs();

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();

//		ImGui::ShowDemoWindow();
		struct UniformBuffer
		{
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			glm::vec3 camPos = glm::vec3(0.0f, 3.0f, -3.0f);
		};

		UniformBuffer uniform_buffer{ };
		uniform_buffer.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		uniform_buffer.model = glm::rotate(uniform_buffer.model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		uniform_buffer.model = glm::rotate(uniform_buffer.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		uniform_buffer.view = glm::lookAt(uniform_buffer.camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		uniform_buffer.projection = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 15.0f);
		uniform_buffer.projection[1][1] *= -1;

		void* data = uniform_buffer_memory_.mapMemory(0, sizeof(UniformBuffer));
		std::memcpy(data, &uniform_buffer, sizeof(UniformBuffer));
		uniform_buffer_memory_.unmapMemory();

		ImGui::Render();

		vk::resultCheck(
			env_->device().waitForFences(*frame_fences[frame_index], true, UINT64_MAX),
			"Error Waiting for frame fence");

		const auto [acquisition_result, image_index] = display_->vk_swapchain()
		                                                       .acquireNextImage(UINT64_MAX, *image_acquisition_semaphores[frame_index]);
		vk::resultCheck(acquisition_result, "Error acquiring image");

		env_->device().resetFences(*frame_fences[frame_index]);

		frame(image_index);

		std::vector<vk::PipelineStageFlags> stages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::SubmitInfo submit_info(
			*image_acquisition_semaphores[frame_index],
			stages,
			*cmds_[frame_index],
			*image_presentation_semaphores[frame_index]);
		env_->graphics_queue().submit(submit_info, *frame_fences[frame_index]);

		vk::PresentInfoKHR present_info(
			*image_presentation_semaphores[frame_index],
			*display_->vk_swapchain(),
			image_index);

		try
		{
			vk::resultCheck(env_->graphics_queue().presentKHR(present_info), "Presentation Failed!");
		}
		catch (const std::exception& e)
		{
			int x, y;
			do
			{
				glfwGetFramebufferSize(wnd_, &x, &y);
				glfwWaitEvents();
			} while (x == 0 || y == 0);

			env_->device().waitIdle();

			display_->recreate();

			framebuffers_.clear();
			for (int i = 0; i < display_->image_views().size(); i++)
			{
				std::vector<vk::ImageView> attachments = {
					*display_->image_views()[i],
					*depth_view_
				};
				vk::FramebufferCreateInfo framebuffer_create_info(
					{ },
					*render_pass_,
					attachments,
					display_->image_extent().width,
					display_->image_extent().height,
					1);
				framebuffers_.push_back(env_->device().createFramebuffer(framebuffer_create_info));
			}
		}


		frame_index = (frame_index + 1) % total_frames;
	}
	env_->device().waitIdle();
}

void small_application::frame(int image_index)
{
	vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	std::vector<vk::ClearValue> clear_values = {
		vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 1.0f, 0.5f, 0.5f, 1.0f })),
		vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 1)),
	};
	vk::RenderPassBeginInfo render_pass_begin_info(
		*render_pass_,
		*framebuffers_[image_index],
		vk::Rect2D(vk::Offset2D(0, 0), display_->image_extent()),
		clear_values);

	cmds_[frame_index].reset();
	cmds_[frame_index].begin(begin_info);
	cmds_[frame_index].beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

	vk::Viewport viewport(
		0.0f, 0.0f,
		display_->image_extent().width, display_->image_extent().height,
		0.0f, 1.0f);
	cmds_[frame_index].setViewport(0, viewport);

	vk::Rect2D scissor(
		vk::Offset2D(0, 0),
		vk::Extent2D(display_->image_extent().width, display_->image_extent().height));
	cmds_[frame_index].setScissor(0, scissor);

	cmds_[frame_index].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline_);
	cmds_[frame_index].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout_, 0, *descriptor_set_, { });

	aos_mesh_primitive_.draw(cmds_[frame_index]);
	
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmds_[frame_index]);
	cmds_[frame_index].endRenderPass();
	cmds_[frame_index].end();
}

} // gorilla