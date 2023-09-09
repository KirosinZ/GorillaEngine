#include <vulkan_renderer/physical_device.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

physical_device::physical_device(std::shared_ptr<vk::raii::PhysicalDevice> physical_device)
	: physical_device_(physical_device)
{
	properties_ = physical_device->getProperties2();
	features_ = physical_device->getFeatures2();
	memory_properties_ = physical_device->getMemoryProperties2();
	queue_families_ = physical_device->getQueueFamilyProperties2();
}

}