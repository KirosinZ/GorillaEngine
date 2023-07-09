#include <iostream>

#include "platform/common.hpp"

#include <vulkan/vulkan_raii.hpp>

#include <glfw_cpp/glfw.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>

#include <thread>


#include <error_handling/exceptions.hpp>
#include <error_handling/assert.hpp>

#include <glfw_cpp/monitor.hpp>

int main2()
{
	std::ostream::sync_with_stdio(false);
	glfw::library glfw_handle{};
	glfw::monitor::set_config_callback([] (glfw::monitor& mnt, int event)
	{
		if (event == GLFW_CONNECTED)
			std::cout << "New Monitor Connected!" << std::endl;
		else
		{
			if (mnt.user_data().has_value())
				std::cout << "Monitor Died with user data = " << std::any_cast<i32>(mnt.user_data()) << std::endl;
			else
				std::cout << "Monitor Died without user data" << std::endl;
		}

	});

	while (true)
		glfwPollEvents();
	return 0;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* wnd = glfwCreateWindow(1920, 1080, "Gorillion", nullptr, nullptr);
	glfwSetWindowAttrib(wnd, GLFW_FOCUSED, GLFW_TRUE);
	std::cout << glfwGetWindowAttrib(wnd, GLFW_FOCUSED) << std::endl;
	glfwSetWindowAttrib(wnd, GLFW_FOCUSED, GLFW_FALSE);
	std::cout << glfwGetWindowAttrib(wnd, GLFW_FOCUSED) << std::endl;
	glfwSetWindowShouldClose(wnd, GLFW_TRUE);

	std::vector<const char*> required_extensions;
	u32 cnt = 0;
	glfwGetRequiredInstanceExtensions(&cnt);
	required_extensions.resize(cnt);
	const auto* ptr = glfwGetRequiredInstanceExtensions(&cnt);
	for (int i = 0; i < cnt; i++)
	{
		required_extensions[i] = ptr[i];
	}

	vk::ApplicationInfo application_info(
			"Gorillion",
			VK_MAKE_VERSION(0, 0, 1),
			"Gorilla Engine",
			VK_MAKE_VERSION(build::version_major, build::version_minor, build::version_patch));

	std::vector<const char*> layers{
			"VK_LAYER_KHRONOS_validation",
	};

	std::vector<const char*> extensions = required_extensions;

	vk::InstanceCreateInfo instance_create_info(
			{},
			&application_info,
			layers,
			extensions);

	vk::raii::Context context;
	vk::raii::Instance instance = context.createInstance(instance_create_info);

	VkSurfaceKHR surface_ptr;
	glfwCreateWindowSurface(*instance, wnd, nullptr, &surface_ptr);
	vk::raii::SurfaceKHR surface(instance, surface_ptr);

	std::vector<vk::raii::PhysicalDevice> phys_devices = instance.enumeratePhysicalDevices();

	vk::raii::PhysicalDevice phys_device = phys_devices[0];

	i32 family_index = -1;
	const auto& families = phys_device.getQueueFamilyProperties();
	for (int i = 0; i < families.size(); i++) {
		if (families[i].queueFlags & vk::QueueFlagBits::eGraphics && phys_device.getSurfaceSupportKHR(i, *surface))
			family_index = i;
	}

	std::vector<float> priorities = { 1.0f };
	vk::DeviceQueueCreateInfo device_queue_create_info(
			{},
			family_index,
			priorities);

	std::vector<const char*> device_layers{};

	std::vector<const char*> device_extensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vk::PhysicalDeviceFeatures features{};
	features.samplerAnisotropy = true;

	vk::DeviceCreateInfo device_create_info(
			{},
			device_queue_create_info,
			device_layers,
			device_extensions,
			&features);
	vk::raii::Device device = phys_device.createDevice(device_create_info);

	vk::raii::Queue queue = device.getQueue(family_index, 0);

	vk::SurfaceCapabilitiesKHR surface_capabilities = phys_device.getSurfaceCapabilitiesKHR(*surface);
	std::vector<vk::SurfaceFormatKHR> surface_formats = phys_device.getSurfaceFormatsKHR(*surface);
	std::vector<vk::PresentModeKHR> present_modes =  phys_device.getSurfacePresentModesKHR(*surface);

	const u32 min_image_count = std::min(std::max(2u, surface_capabilities.minImageCount), surface_capabilities.maxImageCount);

	vk::Format chosen_one = surface_formats[0].format;
	vk::ColorSpaceKHR chosen_two = surface_formats[0].colorSpace;
//	for (auto& format : surface_formats)
//	{
//		if ()
//		{
//			chosen_one = format.format;
//			chosen_two = format.colorSpace;
//		}
//	}

	vk::PresentModeKHR chosen_three{};
	for (auto& mode : present_modes)
	{
		if (mode == vk::PresentModeKHR::eFifo) {
			chosen_three = mode;
		}
	}

	const u32 w = std::clamp(
			surface_capabilities.currentExtent.width,
			surface_capabilities.minImageExtent.width,
			surface_capabilities.maxImageExtent.width);
	const u32 h = std::clamp(
			surface_capabilities.currentExtent.height,
			surface_capabilities.minImageExtent.height,
			surface_capabilities.maxImageExtent.height);
	vk::Extent2D extent{ w, h };

	std::vector<u32> family_indices{ u32(family_index) };
	vk::SwapchainCreateInfoKHR swapchain_create_info(
			{},
			*surface,
			min_image_count,
			chosen_one,
			chosen_two,
			extent,
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			family_indices,
			vk::SurfaceTransformFlagBitsKHR::eIdentity,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			chosen_three);
	vk::raii::SwapchainKHR swapchain = device.createSwapchainKHR(swapchain_create_info);
	std::vector<VkImage> sc_images_raw = swapchain.getImages();
	std::vector<vk::Image> sc_images;
	sc_images.reserve(sc_images_raw.size());
	for (auto& image : sc_images_raw)
	{
		sc_images.emplace_back(image);
	}
	sc_images_raw.clear();

	std::vector<vk::raii::ImageView> sc_image_views;
	for (int i = 0; i < sc_images.size(); ++i)
	{
		vk::ImageViewCreateInfo create_info(
				{},
				sc_images[i],
				vk::ImageViewType::e2D,
				chosen_one,
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(
						vk::ImageAspectFlagBits::eColor,
						0,
						1,
						0,
						1));
		sc_image_views.emplace_back(device, create_info);
	}

	vk::CommandPoolCreateInfo command_pool_create_info(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			family_index);
	vk::raii::CommandPool command_pool = device.createCommandPool(command_pool_create_info);

	vk::AttachmentDescription color_attachment(
			{},
			chosen_one,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR);

	vk::Format depth_format{};
	std::vector<vk::Format> possible_formats = {
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD16UnormS8Uint,
			vk::Format::eD24UnormS8Uint,
	};
	for (const auto format : possible_formats)
	{
		vk::FormatProperties props = phys_device.getFormatProperties(format);
		if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			depth_format = format;
			break;
		}
	}

	vk::AttachmentDescription depth_attachment(
			{},
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
	vk::raii::RenderPass render_pass = device.createRenderPass(render_pass_create_info);

	vk::ImageCreateInfo depth_image_create_info(
			{},
			vk::ImageType::e2D,
			depth_format,
			vk::Extent3D(extent.width, extent.height, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive,
			{ },
			vk::ImageLayout::eUndefined);
	vk::raii::Image depth_image = device.createImage(depth_image_create_info);


	vk::ImageMemoryRequirementsInfo2 image_mem_req(*depth_image);
	vk::MemoryRequirements2 memory_requirements_2 = device.getImageMemoryRequirements2(image_mem_req);
	u32 memory_type = 0;
	vk::PhysicalDeviceMemoryProperties memory_properties = phys_device.getMemoryProperties();
	for (int i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if (memory_requirements_2.memoryRequirements.memoryTypeBits & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal)
			memory_type = i;
	}
	vk::MemoryAllocateInfo memory_allocate_info(
			memory_requirements_2.memoryRequirements.size,
			memory_type);
	vk::raii::DeviceMemory depth_memory = device.allocateMemory(memory_allocate_info);

	vk::BindImageMemoryInfo bind_image_memory_info(
			*depth_image,
			*depth_memory,
			0);
	device.bindImageMemory2(bind_image_memory_info);

	vk::ImageViewCreateInfo depth_view_create_info(
			{},
			*depth_image,
			vk::ImageViewType::e2D,
			depth_format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eDepth,
					0, 1, 0, 1));
	vk::raii::ImageView depth_image_view = device.createImageView(depth_view_create_info);

	std::vector<vk::raii::Framebuffer> framebuffers;
	for (int i = 0; i < sc_image_views.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {
				*sc_image_views[i],
				*depth_image_view,
		};
		vk::FramebufferCreateInfo framebuffer_create_info(
				{},
				*render_pass,
				attachments,
				extent.width,
				extent.height,
				1);
		framebuffers.push_back(device.createFramebuffer(framebuffer_create_info));
	}

	vk::CommandBufferAllocateInfo allocate_info(
			*command_pool,
			vk::CommandBufferLevel::ePrimary,
			1);
	vk::raii::CommandBuffer cmd = std::move(device.allocateCommandBuffers(allocate_info)[0]);

	std::vector<vk::DescriptorPoolSize> sizes = {
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
	};

	vk::DescriptorPoolCreateInfo descriptor_pool_create_info(
			{},
			10,
			sizes);
	vk::raii::DescriptorPool imgui_descriptor_pool = device.createDescriptorPool(descriptor_pool_create_info);

	ImGui_ImplVulkan_InitInfo init_info(
			*instance,
			*phys_device,
			*device,
			family_index,
			*queue,
			VK_NULL_HANDLE,
			*imgui_descriptor_pool,
			0,
			min_image_count,
			min_image_count,
			VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
			nullptr,
			nullptr);

	ImGuiContext* imgui_context = ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(wnd, true);
	ImGui_ImplVulkan_Init(&init_info, *render_pass);

	vk::CommandBufferBeginInfo command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(command_buffer_begin_info);

	ImGui_ImplVulkan_CreateFontsTexture(*cmd);

	cmd.end();

	vk::SemaphoreCreateInfo semaphore_create_info{};
	vk::raii::Semaphore acquire_semaphore = device.createSemaphore(semaphore_create_info);
	vk::raii::Semaphore present_semaphore = device.createSemaphore(semaphore_create_info);

	vk::SubmitInfo submit_info(
			{},
			{},
			*cmd);
	queue.submit(submit_info);
	queue.waitIdle();

	command_buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	int frame_num = 0;

	std::chrono::time_point ts_start = std::chrono::steady_clock::now();
	while(!glfwWindowShouldClose(wnd))
	{
		while(glfwGetWindowAttrib(wnd, GLFW_ICONIFIED) == GLFW_TRUE)
			glfwPollEvents();

		glfwPollEvents();

		if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(wnd, GLFW_TRUE);
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		vk::Result result_1;
		u32 index;
		std::tie(result_1, index) = swapchain.acquireNextImage(1000, *acquire_semaphore);

		vk::resultCheck(result_1, "crap");

		std::vector<vk::ClearValue> clear_values = {
				vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.5f, 0.5f, 0.5f, 1.0f})),
				vk::ClearValue(vk::ClearDepthStencilValue(0.0f, 1)),
		};
		vk::RenderPassBeginInfo render_pass_begin_info(
				*render_pass,
				*framebuffers[index],
				vk::Rect2D(vk::Offset2D(0, 0), extent),
				clear_values);

		cmd.reset();
		cmd.begin(command_buffer_begin_info);
		cmd.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

		vk::Viewport viewport(
				0.0f, 0.0f,
				w, h,
				0.0f, 1.0f);
		cmd.setViewport(0, viewport);

		vk::Rect2D scissor(
				vk::Offset2D(0, 0),
				vk::Extent2D(w, h));
		cmd.setScissor(0, scissor);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
		cmd.endRenderPass();
		cmd.end();

		std::vector<vk::PipelineStageFlags> stages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submit_info = vk::SubmitInfo(
				*acquire_semaphore,
				stages,
				*cmd,
				*present_semaphore);
		queue.submit(submit_info);
		vk::PresentInfoKHR present_info(
				*present_semaphore,
				*swapchain,
				index);
		try
		{
			vk::resultCheck(queue.presentKHR(present_info), "present failed");
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Nonsense" << std::endl;
		}

		std::chrono::time_point ts_now = std::chrono::steady_clock::now();
		int spf = std::chrono::duration_cast<std::chrono::microseconds>(ts_now - ts_start).count();
		float fps =float(1'000'000) / spf;
		std::cout << fps << std::endl;
		ts_start = ts_now;

		++frame_num;
		queue.waitIdle();
	}

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(imgui_context);
	glfwDestroyWindow(wnd);

	return 0;
}

int main()
{
	try
	{
		return main2();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unhandled generic std::exception! Message: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Fallback catch-clause reached" << std::endl;
	}
}
