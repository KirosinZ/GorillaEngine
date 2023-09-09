#ifndef GORILLA_PHYSICAL_DEVICE_HPP
#define GORILLA_PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <ranges>


namespace gorilla::vulkan_renderer::boilerplate
{

class physical_device
{
public:
	physical_device() = default;

	physical_device(std::shared_ptr<vk::raii::PhysicalDevice> physical_device);

	inline vk::PhysicalDeviceProperties properties() const
	{ return properties_.properties; }

	inline vk::PhysicalDeviceLimits limits() const
	{ return properties_.properties.limits; }

	inline vk::PhysicalDeviceFeatures features() const
	{ return features_.features; }

	inline const auto& memory_types() const
	{ return memory_properties_.memoryProperties.memoryTypes; }

	inline const auto& memory_heaps() const
	{ return memory_properties_.memoryProperties.memoryHeaps; }

	inline auto queue_families() const
	{ return queue_families_ | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties); }

public:
	inline operator const vk::raii::PhysicalDevice&() const
	{ return *physical_device_; }

	inline const vk::PhysicalDevice& operator*() const
	{ return *(*physical_device_); }

	inline const vk::raii::PhysicalDevice* operator->() const
	{ return physical_device_.operator->(); }

private:
	std::shared_ptr<vk::raii::PhysicalDevice> physical_device_ = nullptr;

	vk::PhysicalDeviceProperties2 properties_{ };
	vk::PhysicalDeviceFeatures2 features_{ };
	vk::PhysicalDeviceMemoryProperties2 memory_properties_{ };
	std::vector<vk::QueueFamilyProperties2> queue_families_{ };
};

}

#endif //GORILLA_PHYSICAL_DEVICE_HPP
