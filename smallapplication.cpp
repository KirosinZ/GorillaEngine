#include "smallapplication.hpp"

#include <iostream>

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

	const std::vector<glm::vec3> vertices = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),

		glm::vec3(0.5f, -0.5f, 0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),

		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),

		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),

		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, 0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),

		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
	};
	const std::vector<glm::vec3> normals = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
	};
	const std::vector<int32_t> indices = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23,
	};
	vertex_byte_size_ = sizeof(glm::vec3) * vertices.size();
	n_indices_ = indices.size();
	geom cube(vertices, normals, indices);

//	vk::VertexInputBindingDescription vertex_binding_description(
//		0,
//		2 * sizeof(glm::vec3),
//		vk::VertexInputRate::eVertex);
//
//	std::vector<vk::VertexInputAttributeDescription> vertex_attribute_descriptions = {
//		vk::VertexInputAttributeDescription(
//			0,
//			0,
//			vk::Format::eR32G32B32Sfloat,
//			0),
//		vk::VertexInputAttributeDescription(
//			1,
//			0,
//			vk::Format::eR32G32B32Sfloat,
//			sizeof(glm::vec3)),
//	};
	const auto bindings = cube.binding_descriptions();
	const auto attributes = cube.attribute_descriptions();
	vk::PipelineVertexInputStateCreateInfo vertex_input_info(
		{ },
		bindings,
		attributes);

	vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
		{ },
		vk::PrimitiveTopology::eTriangleList,
		false);

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
		vk::DynamicState::eScissor
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

	vk::BufferCreateInfo vertex_buffer_create_info(
		{ },
		sizeof(glm::vec3) * (cube.vertices().size() + cube.normals().size()),
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		gfs);
	vertex_buffer_ = env_->device().createBuffer(vertex_buffer_create_info);

	vk::BufferMemoryRequirementsInfo2 reqs(*vertex_buffer_);
	vk::MemoryRequirements2 vertex_buffer_memory_requirements = env_->device().getBufferMemoryRequirements2(reqs);
	for (int mti = 0; mti < env_->phys_device().getMemoryProperties(); mti++)
	{
		vk::MemoryType type = env_->phys_device().getMemoryProperties().memoryTypes[mti];
		if (vertex_buffer_memory_requirements.memoryRequirements.memoryTypeBits & (1 << mti)
		    && (type.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		       == vk::MemoryPropertyFlagBits::eDeviceLocal)
		{
			mem_type_index = mti;
			break;
		}
	}

	vk::MemoryAllocateInfo vertex_buffer_memory_allocate_info(
		vertex_buffer_memory_requirements.memoryRequirements.size,
		mem_type_index);
	vertex_buffer_memory_ = env_->device().allocateMemory(vertex_buffer_memory_allocate_info);

	vk::BufferCreateInfo index_buffer_create_info(
		{ },
		sizeof(int32_t) * cube.triangle_indices().size(),
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		gfs);
	index_buffer_ = env_->device().createBuffer(index_buffer_create_info);

	reqs = vk::BufferMemoryRequirementsInfo2(*index_buffer_);
	vk::MemoryRequirements2 index_buffer_memory_requirements = env_->device().getBufferMemoryRequirements2(reqs);
	for (int mti = 0; mti < env_->phys_device().getMemoryProperties().memoryTypeCount; mti++)
	{
		vk::MemoryType type = env_->phys_device().getMemoryProperties().memoryTypes[mti];
		if (index_buffer_memory_requirements.memoryRequirements.memoryTypeBits & (1 << mti)
		    && (type.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		       == vk::MemoryPropertyFlagBits::eDeviceLocal)
		{
			mem_type_index = mti;
			break;
		}
	}

	vk::MemoryAllocateInfo index_buffer_memory_allocate_info(
		index_buffer_memory_requirements.memoryRequirements.size,
		mem_type_index);
	index_buffer_memory_ = env_->device().allocateMemory(index_buffer_memory_allocate_info);

	vk::BufferCreateInfo staging_buffer_create_info(
		{ },
		sizeof(glm::vec3) * (cube.vertices().size() + cube.normals().size()) + sizeof(int32_t) * indices.size(),
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		gfs);
	vk::raii::Buffer staging_buffer = env_->device().createBuffer(staging_buffer_create_info);

	reqs = vk::BufferMemoryRequirementsInfo2(*staging_buffer);
	vk::MemoryRequirements2 staging_buffer_memory_requirements = env_->device().getBufferMemoryRequirements2(reqs);
	for (int mti = 0; mti < env_->phys_device().getMemoryProperties().memoryTypeCount; mti++)
	{
		vk::MemoryType type = env_->phys_device().getMemoryProperties().memoryTypes[mti];
		if (staging_buffer_memory_requirements.memoryRequirements.memoryTypeBits & (1 << mti)
		    && (type.propertyFlags
		        & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
		       == (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
		{
			mem_type_index = mti;
			break;
		}
	}
	vk::MemoryAllocateInfo staging_buffer_memory_allocate_info(
		staging_buffer_memory_requirements.memoryRequirements.size,
		mem_type_index);
	vk::raii::DeviceMemory staging_buffer_memory = env_->device().allocateMemory(staging_buffer_memory_allocate_info);

	std::vector<vk::BindBufferMemoryInfo> bind_infos = {
		vk::BindBufferMemoryInfo{
			*vertex_buffer_,
			*vertex_buffer_memory_,
			0
		},
		vk::BindBufferMemoryInfo{
			*index_buffer_,
			*index_buffer_memory_,
			0
		},
		vk::BindBufferMemoryInfo{
			*staging_buffer,
			*staging_buffer_memory,
			0
		}
	};
	env_->device().bindBufferMemory2(bind_infos);

	void* data = staging_buffer_memory.mapMemory(0, sizeof(glm::vec3) * cube.vertices().size());
	std::memcpy(data, cube.vertices().data(), sizeof(glm::vec3) * cube.vertices().size());
	staging_buffer_memory.unmapMemory();
	data = staging_buffer_memory.mapMemory(
		sizeof(glm::vec3) * cube.vertices().size(), sizeof(glm::vec3)
		                                            * cube.normals().size());
	std::memcpy(data, cube.normals().data(), sizeof(glm::vec3) * cube.normals().size());
	staging_buffer_memory.unmapMemory();

	vk::BufferCopy vertex_copy_region(0, 0, sizeof(glm::vec3) * (cube.vertices().size() + cube.normals().size()));

	tmp_cmd.reset();
	tmp_cmd.begin(command_buffer_begin_info);
	tmp_cmd.copyBuffer(*staging_buffer, *vertex_buffer_, vertex_copy_region);
	tmp_cmd.end();

	submit_info = vk::SubmitInfo(
		{ },
		{ },
		*tmp_cmd,
		{ });
	env_->graphics_queue().submit(submit_info);
	env_->graphics_queue().waitIdle();

	data = staging_buffer_memory.mapMemory(0, sizeof(int32_t) * cube.triangle_indices().size());
	std::memcpy(data, indices.data(), sizeof(int32_t) * cube.triangle_indices().size());
	staging_buffer_memory.unmapMemory();

	vk::BufferCopy index_copy_region(0, 0, sizeof(int32_t) * cube.triangle_indices().size());

	tmp_cmd.reset();
	tmp_cmd.begin(command_buffer_begin_info);
	tmp_cmd.copyBuffer(*staging_buffer, *index_buffer_, index_copy_region);
	tmp_cmd.end();

	submit_info = vk::SubmitInfo(
		{ },
		{ },
		*tmp_cmd,
		{ });
	env_->graphics_queue().submit(submit_info);
	env_->graphics_queue().waitIdle();

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

	reqs = vk::BufferMemoryRequirementsInfo2(*uniform_buffer_);
	vk::MemoryRequirements2 uniform_buffer_memory_requirements = env_->device().getBufferMemoryRequirements2(reqs);
	for (int mti = 0; mti < env_->phys_device().getMemoryProperties().memoryTypeCount; mti++)
	{
		vk::MemoryType type = env_->phys_device().getMemoryProperties().memoryTypes[mti];
		if (uniform_buffer_memory_requirements.memoryRequirements.memoryTypeBits & (1 << mti)
		    && (type.propertyFlags
		        & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
		       == (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
		{
			mem_type_index = mti;
			break;
		}
	}

	vk::MemoryAllocateInfo uniform_buffer_memory_allocate_info(
		uniform_buffer_memory_requirements.memoryRequirements.size,
		mem_type_index);
	uniform_buffer_memory_ = env_->device().allocateMemory(uniform_buffer_memory_allocate_info);

	vk::BindBufferMemoryInfo uniform_buffer_bind_info(*uniform_buffer_, *uniform_buffer_memory_, 0);
	env_->device().bindBufferMemory2(uniform_buffer_bind_info);

	data = uniform_buffer_memory_.mapMemory(0, sizeof(UniformBuffer));
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

			glm::vec3 camPos = glm::vec3(0.0f, 1.0f, -1.0f);
		};

		UniformBuffer uniform_buffer{ };
		uniform_buffer.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
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
	cmds_[frame_index].bindVertexBuffers(0, { *vertex_buffer_, *vertex_buffer_ }, { 0, uint32_t(vertex_byte_size_) });
	cmds_[frame_index].bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);

	cmds_[frame_index].drawIndexed(n_indices_, 1, 0, 0, 0);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmds_[frame_index]);
	cmds_[frame_index].endRenderPass();
	cmds_[frame_index].end();
}

} // gorilla