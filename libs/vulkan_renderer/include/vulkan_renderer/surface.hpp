#ifndef GORILLA_SURFACE_HPP
#define GORILLA_SURFACE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <vulkan_renderer/physical_device.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

class surface
{
public:
	surface() = default;

	surface(const physical_device& physical_device, std::shared_ptr<vk::raii::SurfaceKHR> surface);

	inline vk::SurfaceCapabilitiesKHR capabilities() const
	{ return capabilities_.surfaceCapabilities; }

	inline auto formats() const
	{ return formats_ | std::views::transform(&vk::SurfaceFormat2KHR::surfaceFormat); }

	inline const std::vector<vk::PresentModeKHR>& present_modes() const
	{ return present_modes_; }

public:
	inline operator const vk::raii::SurfaceKHR&() const
	{ return *surface_; }

	inline const vk::SurfaceKHR& operator*() const
	{ return *(*surface_); }

	inline const vk::raii::SurfaceKHR* operator->() const
	{ return surface_.operator->(); }

private:
	std::shared_ptr<vk::raii::SurfaceKHR> surface_ = nullptr;

	vk::SurfaceCapabilities2KHR capabilities_{ };
	std::vector<vk::SurfaceFormat2KHR> formats_{ };
	std::vector<vk::PresentModeKHR> present_modes_{ };
};

}

#endif //GORILLA_SURFACE_HPP
