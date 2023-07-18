#include <iostream>

#include "platform/common.hpp"

#include <vulkan/vulkan_raii.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>

#include <thread>


#include <error_handling/exceptions.hpp>
#include <error_handling/assert.hpp>

//#include <glfw_cpp/glfw.hpp>
#include <glfw_cpp/vulkan_glue.hpp>
#include <glfw_cpp/glfw.hpp>

#include <vulkan_renderer/environment.hpp>
#include <vulkan_renderer/display.hpp>


int main()
{
	glfw::library glfw_handle = *glfw::initialize();

	glfw::window window = *glfw::window::ctor(1920, 1080, "Gorillionaire", {
			.window_hints = {
					.resizable = true,
					.maximized = true,
			},
			.framebuffer_hints = {
					.doublebuffer = false
			},
	});

	vkr::environment env = *vkr::environment::ctor({
			.app_name = window.title(),
			.desired_instance_layers = { "VK_LAYER_KHRONOS_validation" },
			.desired_instance_extensions = glfw::required_instance_extensions(),
			.desired_device_extensions = { "VK_KHR_swapchain" }
	});

	vkr::display display = *vkr::display::ctor(env, window);


	vk::CommandPoolCreateInfo command_pool_create_info(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			env.graphics_family());
	vk::raii::CommandPool command_pool = env.device().createCommandPool(command_pool_create_info);

	vk::AttachmentDescription color_attachment(
			{},
			display.image_format(),
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentDescription depth_attachment(
			{},
			display.depth_format(),
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
			{},
			vk::PipelineBindPoint::eGraphics,
			{},
			color_ref,
			{},
			&depth_ref,
			{});

	std::vector<vk::AttachmentDescription> attachments = {
			color_attachment,
			depth_attachment,
	};

	vk::SubpassDependency dependency(
			VK_SUBPASS_EXTERNAL,
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			{},
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	vk::RenderPassCreateInfo render_pass_create_info(
			{},
			attachments,
			ui_pass,
			dependency);
	vk::raii::RenderPass render_pass = env.device().createRenderPass(render_pass_create_info);

	std::vector<vk::raii::Framebuffer> framebuffers;
	for (int i = 0; i < display.image_views().size(); i++)
	{
		std::vector<vk::ImageView> attachments = {
				*display.image_views()[i],
				*display.depth_image_view(),
		};
		vk::FramebufferCreateInfo framebuffer_create_info(
				{},
				*render_pass,
				attachments,
				display.image_extent().width,
				display.image_extent().height,
				1);
		framebuffers.push_back(env.device().createFramebuffer(framebuffer_create_info));
	}

	vk::CommandBufferAllocateInfo allocate_info(
			*command_pool,
			vk::CommandBufferLevel::ePrimary,
			1);
	vk::raii::CommandBuffer cmd = std::move(env.device().allocateCommandBuffers(allocate_info)[0]);

	std::vector<vk::DescriptorPoolSize> sizes = {
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
	};

	vk::DescriptorPoolCreateInfo descriptor_pool_create_info(
			{},
			10,
			sizes);
	vk::raii::DescriptorPool imgui_descriptor_pool = env.device().createDescriptorPool(descriptor_pool_create_info);

	ImGui_ImplVulkan_InitInfo init_info(
			*env.instance(),
			*env.phys_device(),
			*env.device(),
			env.graphics_family(),
			*env.graphics_queue(),
			VK_NULL_HANDLE,
			*imgui_descriptor_pool,
			0,
			display.image_views().size(),
			display.image_views().size(),
			VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
			nullptr,
			nullptr);

	ImGuiContext* imgui_context = ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(window.handle(), true);
	ImGui_ImplVulkan_Init(&init_info, *render_pass);

	vk::CommandBufferBeginInfo command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(command_buffer_begin_info);

	ImGui_ImplVulkan_CreateFontsTexture(*cmd);

	cmd.end();

	vk::SemaphoreCreateInfo semaphore_create_info{};
	std::vector<vk::raii::Semaphore> acquire_semaphores;
	acquire_semaphores.push_back(env.device().createSemaphore(semaphore_create_info));
	acquire_semaphores.push_back(env.device().createSemaphore(semaphore_create_info));

	std::vector<vk::raii::Semaphore> present_semaphores{};
	present_semaphores.push_back(env.device().createSemaphore(semaphore_create_info));
	present_semaphores.push_back(env.device().createSemaphore(semaphore_create_info));

	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	std::vector<vk::raii::Fence> is_flight_fences{};
	is_flight_fences.push_back(env.device().createFence(fence_create_info));
	is_flight_fences.push_back(env.device().createFence(fence_create_info));

	allocate_info.commandBufferCount = 2;
	std::vector<vk::raii::CommandBuffer> cmds = env.device().allocateCommandBuffers(allocate_info);

	vk::SubmitInfo submit_info(
			{},
			{},
			*cmd);
	env.graphics_queue().submit(submit_info);
	env.graphics_queue().waitIdle();

	command_buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	int frame_num = 0;

	std::chrono::time_point ts_start = std::chrono::steady_clock::now();
	while(!window.should_close())
	{
		glfw::poll_events();

		if (glfwGetKey(window.handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			window.set_should_close(true);
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		vk::resultCheck(env.device().waitForFences(*is_flight_fences[frame_num % 2], true, UINT64_MAX), "is flight fence checker");
		vk::Result result_1;
		u32 index;
		std::tie(result_1, index) = display.swapchain().acquireNextImage(1000, *acquire_semaphores[frame_num % 2]);
		env.device().resetFences(*is_flight_fences[frame_num % 2]);

		std::vector<vk::ClearValue> clear_values = {
				vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{1.0f, 0.5f, 0.5f, 1.0f})),
				vk::ClearValue(vk::ClearDepthStencilValue(0.0f, 1)),
		};
		vk::RenderPassBeginInfo render_pass_begin_info(
				*render_pass,
				*framebuffers[index],
				vk::Rect2D(vk::Offset2D(0, 0), display.image_extent()),
				clear_values);

		cmds[frame_num % 2].reset();
		cmds[frame_num % 2].begin(command_buffer_begin_info);
		cmds[frame_num % 2].beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

		vk::Viewport viewport(
				0.0f, 0.0f,
				display.image_extent().width, display.image_extent().height,
				0.0f, 1.0f);
		cmds[frame_num % 2].setViewport(0, viewport);

		vk::Rect2D scissor(
				vk::Offset2D(0, 0),
				vk::Extent2D(display.image_extent().width, display.image_extent().height));
		cmds[frame_num % 2].setScissor(0, scissor);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmds[frame_num % 2]);
		cmds[frame_num % 2].endRenderPass();
		cmds[frame_num % 2].end();

		std::vector<vk::PipelineStageFlags> stages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submit_info = vk::SubmitInfo(
				*acquire_semaphores[frame_num % 2],
				stages,
				*cmds[frame_num % 2],
				*present_semaphores[frame_num % 2]);
		env.graphics_queue().submit(submit_info, *is_flight_fences[frame_num % 2]);
		vk::PresentInfoKHR present_info(
				*present_semaphores[frame_num % 2],
				*display.swapchain(),
				index);

		try
		{
			env.graphics_queue().waitIdle();
			vk::resultCheck(env.graphics_queue().presentKHR(present_info), "present failed");
		}
		catch (const std::exception& e)
		{
			while(window.framebuffer_size().first == 0 || window.framebuffer_size().second == 0)
				glfw::wait_events();

			env.device().waitIdle();

			display.recreate();

			framebuffers.clear();
			for (int i = 0; i < display.image_views().size(); i++)
			{
				std::vector<vk::ImageView> attachments = {
						*display.image_views()[i],
						*display.depth_image_view(),
				};
				vk::FramebufferCreateInfo framebuffer_create_info(
						{},
						*render_pass,
						attachments,
						display.image_extent().width,
						display.image_extent().height,
						1);
				framebuffers.push_back(env.device().createFramebuffer(framebuffer_create_info));
			}

			++frame_num;
			continue;
		}


		std::chrono::time_point ts_now = std::chrono::steady_clock::now();
		int spf = std::chrono::duration_cast<std::chrono::microseconds>(ts_now - ts_start).count();
		float fps = float(1'000'000) / spf;
		std::cout << fps << std::endl;
		ts_start = ts_now;

		++frame_num;
	}

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(imgui_context);

	return 0;
}
