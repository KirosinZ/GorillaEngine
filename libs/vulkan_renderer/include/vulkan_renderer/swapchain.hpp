#ifndef GORILLA_SWAPCHAIN_HPP
#define GORILLA_SWAPCHAIN_HPP

#include <vulkan/vulkan_raii.hpp>

#include <error_handling/exceptions.hpp>

#include <glfw_cpp/window.hpp>

#include "vulkan_renderer/environment.hpp"


namespace gorilla::vulkan_renderer
{

struct swapchain_create_info
{
	uint32_t desired_image_count = 2;
};

class swapchain
{
public:
	swapchain(std::shared_ptr<environment> env, const vk::SurfaceKHR& surface, const swapchain_create_info& create_info = { });

	swapchain(const swapchain&) = delete;

	swapchain& operator=(const swapchain&) = delete;

	swapchain(swapchain&&) noexcept = default;

	// Will not work correctly, needs proper implementation
	swapchain& operator=(swapchain&&) = default;

	void recreate();

	inline const vk::raii::SurfaceKHR& surface() const noexcept
	{ return surface_; }

	inline const vk::raii::SwapchainKHR& vk_swapchain() const noexcept
	{ return swapchain_; }

	inline const std::vector<vk::raii::ImageView>& image_views() const noexcept
	{ return image_views_; }

	inline const vk::Extent2D image_extent() const noexcept
	{ return image_extent_; }

	inline const vk::Format image_format() const noexcept
	{ return image_format_; }

	inline const vk::ColorSpaceKHR image_color_space() const noexcept
	{ return image_color_space_; }

	inline const vk::PresentModeKHR present_mode() const noexcept
	{ return present_mode_; }

private:
	std::shared_ptr<environment> environment_ = nullptr;

	vk::raii::SurfaceKHR surface_ = nullptr;
	vk::raii::SwapchainKHR swapchain_ = nullptr;
	std::vector<vk::raii::ImageView> image_views_;

	vk::Extent2D image_extent_{ };
	vk::Format image_format_{ };
	vk::ColorSpaceKHR image_color_space_{ };
	vk::PresentModeKHR present_mode_{ };
};

} // vulkan_renderer

#endif //GORILLA_SWAPCHAIN_HPP
