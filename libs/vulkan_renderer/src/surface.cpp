#include <vulkan_renderer/surface.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

surface::surface(
	const physical_device& physical_device,
	std::shared_ptr<vk::raii::SurfaceKHR> surface)
	: surface_(surface)
{
	capabilities_ = physical_device->getSurfaceCapabilities2KHR(*(*surface_));
	vk::PhysicalDeviceSurfaceInfo2KHR physical_device_surface_info(
		*(*surface_));
	formats_ = physical_device->getSurfaceFormats2KHR(physical_device_surface_info);
	present_modes_ = physical_device->getSurfacePresentModesKHR(*(*surface_));
}

}