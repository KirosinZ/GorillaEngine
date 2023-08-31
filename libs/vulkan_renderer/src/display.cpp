#include "vulkan_renderer/display.hpp"

#include <glfw_cpp/vulkan_glue.hpp>


namespace gorilla::vulkan_renderer
{
//display::display(
//		environment& t_environment,
//		vk::raii::SurfaceKHR t_surface,
//		vk::raii::SwapchainKHR t_swapchain,
//		std::vector<vk::Image> t_images,
//		std::vector<vk::raii::ImageView> t_image_views,
//		vk::raii::Image t_depth_image,
//		vk::raii::ImageView t_depth_view,
//		vk::raii::DeviceMemory t_depth_memory,
//		vk::Extent2D t_extent,
//		vk::Format t_image_format,
//		vk::Format t_depth_format,
//		vk::ColorSpaceKHR t_color_space,
//		vk::PresentModeKHR t_present_mode) noexcept
//		: m_environment(&t_environment),
//		  m_surface(std::move(t_surface)),
//		  m_swapchain(std::move(t_swapchain)),
//		  m_images(std::move(t_images)),
//		  m_image_views(std::move(t_image_views)),
//		  m_depth_image(std::move(t_depth_image)),
//		  m_depth_view(std::move(t_depth_view)),
//		  m_depth_memory(std::move(t_depth_memory)),
//		  m_image_extent(t_extent),
//		  m_image_format(t_image_format),
//		  m_depth_format(t_depth_format),
//		  m_image_color_space(t_color_space),
//		  m_present_mode(t_present_mode)
//{}


gorilla::error_handling::result<display> display::ctor(environment& env, glfw::window& t_window, const display_create_info& t_create_info)
{
	display res{ };
	res.m_environment = &env;
	res.m_window = &t_window;
	res.m_surface = glfw::surface(env.instance(), t_window);

	vk::SurfaceCapabilitiesKHR surface_capabilities = env.phys_device().getSurfaceCapabilitiesKHR(*res.m_surface);
	std::vector<vk::SurfaceFormatKHR> surface_formats = env.phys_device().getSurfaceFormatsKHR(*res.m_surface);
	std::vector<vk::PresentModeKHR> surface_present_modes = env.phys_device().getSurfacePresentModesKHR(*res.m_surface);

	const uint32_t image_count = std::clamp(
		t_create_info.desired_image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

	res.m_image_format = surface_formats[0].format;
	res.m_image_color_space = surface_formats[0].colorSpace;

	if (std::ranges::any_of(
		surface_present_modes, [](const vk::PresentModeKHR& mode)
		{ return mode == vk::PresentModeKHR::eFifo; }))
		res.m_present_mode = vk::PresentModeKHR::eFifo;

	const uint32_t width = std::clamp(
		surface_capabilities.currentExtent.width,
		surface_capabilities.minImageExtent.width,
		surface_capabilities.maxImageExtent.width);
	const uint32_t height = std::clamp(
		surface_capabilities.currentExtent.height,
		surface_capabilities.minImageExtent.height,
		surface_capabilities.maxImageExtent.height);
	res.m_image_extent = vk::Extent2D{ width, height };


	std::vector<uint32_t> family_indices{ uint32_t(env.graphics_family()) };
	vk::SwapchainCreateInfoKHR swapchain_create_info(
		{ },
		*res.m_surface,
		image_count,
		res.m_image_format,
		res.m_image_color_space,
		res.m_image_extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		family_indices,
		vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		res.m_present_mode);
	res.m_swapchain = env.device().createSwapchainKHR(swapchain_create_info);


	std::vector<VkImage> sc_images_raw = res.m_swapchain.getImages();
	std::vector<vk::Image> sc_images;
	res.m_image_views.reserve(sc_images_raw.size());
	std::ranges::copy(sc_images_raw, std::back_inserter(sc_images));
	sc_images_raw.clear();

	std::vector<vk::raii::ImageView> sc_image_views;
	for (int i = 0; i < sc_images.size(); ++i)
	{
		vk::ImageViewCreateInfo create_info(
			{ },
			sc_images[i],
			vk::ImageViewType::e2D,
			res.m_image_format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1));
		res.m_image_views.emplace_back(env.device(), create_info);
	}

	std::vector<vk::Format> possible_formats = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD24UnormS8Uint,
	};
	for (const auto format : possible_formats)
	{
		vk::FormatProperties props = env.phys_device().getFormatProperties(format);
		if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			res.m_depth_format = format;
			break;
		}
	}

	vk::ImageCreateInfo depth_image_create_info(
		{ },
		vk::ImageType::e2D,
		res.m_depth_format,
		vk::Extent3D(res.m_image_extent.width, res.m_image_extent.height, 1),
		1,
		1,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::SharingMode::eExclusive,
		{ },
		vk::ImageLayout::eUndefined);
	res.m_depth_image = env.device().createImage(depth_image_create_info);


	vk::ImageMemoryRequirementsInfo2 image_mem_req(*res.m_depth_image);
	vk::MemoryRequirements2 memory_requirements_2 = env.device().getImageMemoryRequirements2(image_mem_req);
	uint32_t memory_type = 0;
	vk::PhysicalDeviceMemoryProperties memory_properties = env.phys_device().getMemoryProperties();
	for (int i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if (memory_requirements_2.memoryRequirements.memoryTypeBits & (1 << i)
		    && (memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		       == vk::MemoryPropertyFlagBits::eDeviceLocal)
			memory_type = i;
	}
	vk::MemoryAllocateInfo memory_allocate_info(
		memory_requirements_2.memoryRequirements.size,
		memory_type);
	res.m_depth_memory = env.device().allocateMemory(memory_allocate_info);

	vk::BindImageMemoryInfo bind_image_memory_info(
		*res.m_depth_image,
		*res.m_depth_memory,
		0);
	env.device().bindImageMemory2(bind_image_memory_info);

	vk::ImageViewCreateInfo depth_view_create_info(
		{ },
		*res.m_depth_image,
		vk::ImageViewType::e2D,
		res.m_depth_format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			vk::ImageAspectFlagBits::eDepth,
			0, 1, 0, 1));
	res.m_depth_view = env.device().createImageView(depth_view_create_info);

	return res;
}

void display::recreate()
{
	const int n_images = m_image_views.size();
	m_image_views.clear();

	vk::SurfaceCapabilitiesKHR surface_capabilities = m_environment->phys_device()
	                                                               .getSurfaceCapabilitiesKHR(*m_surface);

	const uint32_t width = std::clamp(
		surface_capabilities.currentExtent.width,
		surface_capabilities.minImageExtent.width,
		surface_capabilities.maxImageExtent.width);
	const uint32_t height = std::clamp(
		surface_capabilities.currentExtent.height,
		surface_capabilities.minImageExtent.height,
		surface_capabilities.maxImageExtent.height);
	m_image_extent = vk::Extent2D{ width, height };


	std::vector<uint32_t> family_indices{ uint32_t(m_environment->graphics_family()) };
	vk::SwapchainCreateInfoKHR swapchain_create_info(
		{ },
		*m_surface,
		n_images,
		m_image_format,
		m_image_color_space,
		m_image_extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		family_indices,
		vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		m_present_mode,
		{ },
		*m_swapchain);
	m_swapchain = m_environment->device().createSwapchainKHR(swapchain_create_info);


	std::vector<VkImage> sc_images_raw = m_swapchain.getImages();
	std::vector<vk::Image> sc_images;
	m_image_views.reserve(sc_images_raw.size());
	std::ranges::copy(sc_images_raw, std::back_inserter(sc_images));
	sc_images_raw.clear();

	for (int i = 0; i < sc_images.size(); ++i)
	{
		vk::ImageViewCreateInfo create_info(
			{ },
			sc_images[i],
			vk::ImageViewType::e2D,
			m_image_format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1));
		m_image_views.emplace_back(m_environment->device(), create_info);
	}

	vk::ImageCreateInfo depth_image_create_info(
		{ },
		vk::ImageType::e2D,
		m_depth_format,
		vk::Extent3D(m_image_extent.width, m_image_extent.height, 1),
		1,
		1,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::SharingMode::eExclusive,
		{ },
		vk::ImageLayout::eUndefined);
	m_depth_image = m_environment->device().createImage(depth_image_create_info);


	vk::ImageMemoryRequirementsInfo2 image_mem_req(*m_depth_image);
	vk::MemoryRequirements2 memory_requirements_2 = m_environment->device().getImageMemoryRequirements2(image_mem_req);
	uint32_t memory_type = 0;
	vk::PhysicalDeviceMemoryProperties memory_properties = m_environment->phys_device().getMemoryProperties();
	for (int i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if (memory_requirements_2.memoryRequirements.memoryTypeBits & (1 << i)
		    && (memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		       == vk::MemoryPropertyFlagBits::eDeviceLocal)
			memory_type = i;
	}
	vk::MemoryAllocateInfo memory_allocate_info(
		memory_requirements_2.memoryRequirements.size,
		memory_type);
	m_depth_memory = m_environment->device().allocateMemory(memory_allocate_info);

	vk::BindImageMemoryInfo bind_image_memory_info(
		*m_depth_image,
		*m_depth_memory,
		0);
	m_environment->device().bindImageMemory2(bind_image_memory_info);

	vk::ImageViewCreateInfo depth_view_create_info(
		{ },
		*m_depth_image,
		vk::ImageViewType::e2D,
		m_depth_format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			vk::ImageAspectFlagBits::eDepth,
			0, 1, 0, 1));
	m_depth_view = m_environment->device().createImageView(depth_view_create_info);
}

} // vulkan_renderer