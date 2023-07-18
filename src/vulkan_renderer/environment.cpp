#include "environment.hpp"

#include <platform/ver.hpp>
#include <glfw_cpp/vulkan_glue.hpp>

namespace vulkan_renderer
{

environment::environment(
		vk::raii::Context t_context,
		vk::raii::Instance t_instance,
		vk::raii::PhysicalDevice t_phys_device,
		vk::raii::Device t_device,
		int t_graphics_family,
		int t_compute_family,
		int t_transfer_family,
		std::vector<vk::raii::Queue> t_queues) noexcept
	: m_context(std::move(t_context)),
	  m_instance(std::move(t_instance)),
	  m_phys_device(std::move(t_phys_device)),
	  m_device(std::move(t_device)),
	  m_graphics_family(t_graphics_family),
	  m_compute_family(t_compute_family),
	  m_transfer_family(t_transfer_family),
	  m_queues(std::move(t_queues))
{}

err::result<environment> environment::ctor(const environment_create_info& create_info)
{
	vk::ApplicationInfo application_info(
			create_info.app_name.c_str(),
			create_info.app_version,
			"Gorilla Engine",
			VK_MAKE_VERSION(build::version_major, build::version_minor, build::version_patch),
			VK_VERSION_1_3);

	std::vector<const char*> instance_layers = create_info.desired_instance_layers;

	std::vector<const char*> instance_extensions = create_info.desired_instance_extensions;

	vk::InstanceCreateInfo instance_create_info(
			{},
			&application_info,
			instance_layers,
			instance_extensions);
	vk::raii::Context context;
	vk::raii::Instance instance = context.createInstance(instance_create_info);

	const std::vector<vk::raii::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

	vk::raii::PhysicalDevice physical_device = physical_devices[0];

	std::vector<int32_t> graphics_families;
	std::vector<int32_t> transfer_families;
	std::vector<int32_t> compute_families;
	const std::vector<vk::QueueFamilyProperties> family_properties = physical_device.getQueueFamilyProperties();
	for (int32_t i = 0; i < family_properties.size(); i++)
	{
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics && glfw::query_present_capabilities(instance, physical_device, i))
			graphics_families.push_back(i);
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eTransfer)
			transfer_families.push_back(i);
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eCompute)
			compute_families.push_back(i);
	}
	int graphics_family = graphics_families[0];
	int i = 0;
	while (compute_families[i] == graphics_family)
		i++;
	int compute_family = compute_families[i];
	i = 0;
	while (transfer_families[i] == graphics_family || transfer_families[i] == compute_family)
		i++;
	int transfer_family = transfer_families[i];

	const std::vector<float> priorities = { 1.0f };
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {
			vk::DeviceQueueCreateInfo({}, graphics_family, priorities),
			vk::DeviceQueueCreateInfo({}, compute_family, priorities),
			vk::DeviceQueueCreateInfo({}, transfer_family, priorities),
	};

	std::vector<const char*> device_layers{};

	std::vector<const char*> device_extensions = create_info.desired_device_extensions;

	vk::DeviceCreateInfo device_create_info(
			{},
			queue_create_infos,
			device_layers,
			device_extensions,
			&create_info.desired_features);
	vk::raii::Device device = physical_device.createDevice(device_create_info);
	std::vector<vk::raii::Queue> queues = {
			device.getQueue(graphics_family, 0),
			device.getQueue(compute_family, 0),
			device.getQueue(transfer_family, 0),
	};

	return environment{
		std::move(context),
		std::move(instance),
		std::move(physical_device),
		std::move(device),
		graphics_family,
		compute_family,
		transfer_family,
		std::move(queues)
	};
}

}