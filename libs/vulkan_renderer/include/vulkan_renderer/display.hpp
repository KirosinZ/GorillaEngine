#ifndef GORILLA_DISPLAY_HPP
#define GORILLA_DISPLAY_HPP

#include <vulkan/vulkan_raii.hpp>

#include <error_handling/exceptions.hpp>

#include <glfw_cpp/window.hpp>

#include "vulkan_renderer/environment.hpp"


namespace gorilla::vulkan_renderer
{

struct display_create_info
{
	uint32_t desired_image_count = 2;
};

class display
{
public:

	display(const display&) = delete;

	display& operator=(const display&) = delete;

	display(display&&) noexcept = default;

	// Will not work correctly, needs proper implementation
	display& operator=(display&&) = default;

	~display() = default;

	static gorilla::error_handling::result<display> ctor(environment& env, glfw::window& t_window, const display_create_info& t_create_info = { });

	void recreate();

	inline const vk::raii::SurfaceKHR& surface() const noexcept
	{ return m_surface; }

	inline const vk::raii::SwapchainKHR& swapchain() const noexcept
	{ return m_swapchain; }

	inline const std::vector<vk::raii::ImageView>& image_views() const noexcept
	{ return m_image_views; }

	inline const vk::raii::Image& depth_image() const noexcept
	{ return m_depth_image; }

	inline const vk::raii::ImageView& depth_image_view() const noexcept
	{ return m_depth_view; }

//	inline const vk::raii::DeviceMemory& depth_image_memory() const noexcept { return m_depth_memory; }
	inline const vk::Extent2D image_extent() const noexcept
	{ return m_image_extent; }

	inline const vk::Format image_format() const noexcept
	{ return m_image_format; }

	inline const vk::Format depth_format() const noexcept
	{ return m_depth_format; }

	inline const vk::ColorSpaceKHR image_color_space() const noexcept
	{ return m_image_color_space; }

	inline const vk::PresentModeKHR present_mode() const noexcept
	{ return m_present_mode; }

private:
	display() = default;

	environment* m_environment = nullptr;
	gorilla::glfw::window* m_window = nullptr;

	vk::raii::SurfaceKHR m_surface = nullptr;
	vk::raii::SwapchainKHR m_swapchain = nullptr;
	std::vector<vk::raii::ImageView> m_image_views;

	vk::raii::Image m_depth_image = nullptr;

	vk::raii::ImageView m_depth_view = nullptr;
	vk::raii::DeviceMemory m_depth_memory = nullptr;

	vk::Extent2D m_image_extent{ };

	vk::Format m_image_format{ };
	vk::Format m_depth_format{ };

	vk::ColorSpaceKHR m_image_color_space{ };
	vk::PresentModeKHR m_present_mode{ };
};

} // vulkan_renderer

#endif //GORILLA_DISPLAY_HPP
