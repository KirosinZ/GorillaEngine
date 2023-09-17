#include "vulkan_renderer/environment.hpp"

#include <glfw_cpp/vulkan_glue.hpp>

#include <vulkan_renderer/device_memory_manager.hpp>


namespace gorilla::vulkan_renderer
{

environment::environment(const environment_create_info& create_info)
{
	vk::ApplicationInfo application_info(
		create_info.app_name.c_str(),
		create_info.app_version,
		"Gorilla Engine",
		VK_MAKE_VERSION(0, 0, 1),
		VK_VERSION_1_3);

	std::vector<const char*> instance_layers = create_info.desired_instance_layers;

	std::vector<const char*> instance_extensions = create_info.desired_instance_extensions;

	vk::InstanceCreateInfo instance_create_info(
		{ },
		&application_info,
		instance_layers,
		instance_extensions);
	vk::raii::Context context;
	instance_ = context.createInstance(instance_create_info);

	const std::vector<vk::raii::PhysicalDevice> physical_devices = instance_.enumeratePhysicalDevices();

	phys_device_ = physical_devices[0];

	std::vector<int32_t> graphics_families;
	std::vector<int32_t> transfer_families;
	std::vector<int32_t> compute_families;
	const std::vector<vk::QueueFamilyProperties> family_properties = phys_device_.getQueueFamilyProperties();
	for (int32_t i = 0; i < family_properties.size(); i++)
	{
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics
		    && glfw::query_present_capabilities(instance_, phys_device_, i))
			graphics_families.push_back(i);
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eTransfer)
			transfer_families.push_back(i);
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eCompute)
			compute_families.push_back(i);
	}

	graphics_family_ = graphics_families[0];
	int i = 0;
	while (compute_families[i] == graphics_family_)
	{
		i++;
	}
	compute_family_ = compute_families[i];
	i = 0;
	while (transfer_families[i] == graphics_family_ || transfer_families[i] == compute_family_)
	{
		i++;
	}
	transfer_family_ = transfer_families[i];

	const std::vector<float> priorities = { 1.0f };
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {
		vk::DeviceQueueCreateInfo({ }, graphics_family_, priorities),
		vk::DeviceQueueCreateInfo({ }, compute_family_, priorities),
		vk::DeviceQueueCreateInfo({ }, transfer_family_, priorities),
	};

	std::vector<const char*> device_layers{ };

	std::vector<const char*> device_extensions = create_info.desired_device_extensions;

	vk::DeviceCreateInfo device_create_info(
		{ },
		queue_create_infos,
		device_layers,
		device_extensions,
		&create_info.desired_features);
	device_ = phys_device_.createDevice(device_create_info);
	queues_ = {
		device_.getQueue(graphics_family_, 0),
		device_.getQueue(compute_family_, 0),
		device_.getQueue(transfer_family_, 0),
	};

	memory_manager_ = device_memory_manager(*this);
}

}