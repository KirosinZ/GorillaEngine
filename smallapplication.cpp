#include "smallapplication.hpp"

#include <iostream>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>


namespace gorilla
{

small_application::small_application(GLFWwindow* wnd, std::shared_ptr<vkr::environment> env, std::shared_ptr<vkr::display> display)
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

//	vk::AttachmentDescription depth_attachment(
//		{ },
//		display_->depth_format(),
//		vk::SampleCountFlagBits::e1,
//		vk::AttachmentLoadOp::eClear,
//		vk::AttachmentStoreOp::eDontCare,
//		vk::AttachmentLoadOp::eDontCare,
//		vk::AttachmentStoreOp::eDontCare,
//		vk::ImageLayout::eUndefined,
//		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference color_ref(
		0,
		vk::ImageLayout::eColorAttachmentOptimal);

//	vk::AttachmentReference depth_ref(
//		1,
//		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription ui_pass(
		{ },
		vk::PipelineBindPoint::eGraphics,
		{ },
		color_ref,
		{ },
		{ },//&depth_ref,
		{ });

	std::vector<vk::AttachmentDescription> attachments = {
		color_attachment,
//		depth_attachment,
	};

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		{ },
		vk::AccessFlagBits::eColorAttachmentWrite /*| vk::AccessFlagBits::eDepthStencilAttachmentWrite*/);

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
//			*display_->depth_image_view(),
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
	vk::raii::DescriptorPool imgui_descriptor_pool = env_->device().createDescriptorPool(descriptor_pool_create_info);

	ImGui_ImplVulkan_InitInfo init_info(
		*env_->instance(),
		*env_->phys_device(),
		*env_->device(),
		env_->graphics_family(),
		*env_->graphics_queue(),
		VK_NULL_HANDLE,
		*imgui_descriptor_pool,
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

		ImGui::ShowDemoWindow();

		ImGui::Render();

		vk::resultCheck(
			env_->device().waitForFences(*frame_fences[frame_index], true, UINT64_MAX),
			"Error Waiting for frame fence");

		const auto [acquisition_result, image_index] = display_->swapchain()
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
			*display_->swapchain(),
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
//					*display_->depth_image_view(),
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
//		vk::ClearValue(vk::ClearDepthStencilValue(0.0f, 1)),
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

//	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmds_[frame_index]);
	cmds_[frame_index].endRenderPass();
	cmds_[frame_index].end();
}

} // gorilla