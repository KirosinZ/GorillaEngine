#include "vulkan_renderer/display.hpp"

#include <glfw_cpp/vulkan_glue.hpp>


namespace gorilla::vulkan_renderer
{

display::display(std::shared_ptr<environment> env, GLFWwindow* wnd, error_handling::error_id& err, const display_create_info& create_info)
	: environment_(env),
	  window_(wnd)
{
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(*environment_->instance(), window_, nullptr, &surface);
	surface_ = vk::raii::SurfaceKHR(environment_->instance(), surface);

	vk::SurfaceCapabilitiesKHR surface_capabilities = environment_->phys_device().getSurfaceCapabilitiesKHR(*surface_);
	std::vector<vk::SurfaceFormatKHR> surface_formats = environment_->phys_device().getSurfaceFormatsKHR(*surface_);
	std::vector<vk::PresentModeKHR> surface_present_modes = environment_->phys_device()
	                                                                    .getSurfacePresentModesKHR(*surface_);

	const uint32_t image_count = std::clamp(
		create_info.desired_image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

	image_format_ = surface_formats[0].format;
	image_color_space_ = surface_formats[0].colorSpace;

	if (std::ranges::any_of(
		surface_present_modes, [](const vk::PresentModeKHR& mode)
		{
			return mode == vk::PresentModeKHR::eFifo;
		}))
		present_mode_ = vk::PresentModeKHR::eFifo;

	const uint32_t width = std::clamp(
		surface_capabilities.currentExtent.width,
		surface_capabilities.minImageExtent.width,
		surface_capabilities.maxImageExtent.width);
	const uint32_t height = std::clamp(
		surface_capabilities.currentExtent.height,
		surface_capabilities.minImageExtent.height,
		surface_capabilities.maxImageExtent.height);
	image_extent_ = vk::Extent2D{ width, height };

	std::vector<uint32_t> family_indices{ uint32_t(environment_->graphics_family()) };
	vk::SwapchainCreateInfoKHR swapchain_create_info(
		{ },
		*surface_,
		image_count,
		image_format_,
		image_color_space_,
		image_extent_,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		family_indices,
		vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		present_mode_);
	swapchain_ = environment_->device().createSwapchainKHR(swapchain_create_info);

	std::vector<VkImage> sc_images_raw = swapchain_.getImages();
	std::vector<vk::Image> sc_images;
	image_views_.reserve(sc_images_raw.size());
	std::ranges::copy(sc_images_raw, std::back_inserter(sc_images));
	sc_images_raw.clear();

	std::vector<vk::raii::ImageView> sc_image_views;
	for (int i = 0; i < sc_images.size(); ++i)
	{
		vk::ImageViewCreateInfo create_info(
			{ },
			sc_images[i],
			vk::ImageViewType::e2D,
			image_format_,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1));
		image_views_.emplace_back(environment_->device(), create_info);
	}
}

void display::recreate()
{
	const int n_images = image_views_.size();
	image_views_.clear();

	vk::SurfaceCapabilitiesKHR surface_capabilities = environment_->phys_device()
	                                                              .getSurfaceCapabilitiesKHR(*surface_);

	const uint32_t width = std::clamp(
		surface_capabilities.currentExtent.width,
		surface_capabilities.minImageExtent.width,
		surface_capabilities.maxImageExtent.width);
	const uint32_t height = std::clamp(
		surface_capabilities.currentExtent.height,
		surface_capabilities.minImageExtent.height,
		surface_capabilities.maxImageExtent.height);
	image_extent_ = vk::Extent2D{ width, height };


	std::vector<uint32_t> family_indices{ uint32_t(environment_->graphics_family()) };
	vk::SwapchainCreateInfoKHR swapchain_create_info(
		{ },
		*surface_,
		n_images,
		image_format_,
		image_color_space_,
		image_extent_,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		family_indices,
		vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		present_mode_,
		{ },
		*swapchain_);
	swapchain_ = environment_->device().createSwapchainKHR(swapchain_create_info);


	std::vector<VkImage> sc_images_raw = swapchain_.getImages();
	std::vector<vk::Image> sc_images;
	image_views_.reserve(sc_images_raw.size());
	std::ranges::copy(sc_images_raw, std::back_inserter(sc_images));
	sc_images_raw.clear();

	for (int i = 0; i < sc_images.size(); ++i)
	{
		vk::ImageViewCreateInfo create_info(
			{ },
			sc_images[i],
			vk::ImageViewType::e2D,
			image_format_,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1));
		image_views_.emplace_back(environment_->device(), create_info);
	}
}

} // vulkan_renderer