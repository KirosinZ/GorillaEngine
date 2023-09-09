#include "vulkan_renderer/instance.hpp"


namespace gorilla::vulkan_renderer::boilerplate
{

instance::instance(std::shared_ptr<vk::raii::Instance> instance)
	: instance_(instance)
{}

instance::instance(
	const std::string& app_name,
	const uint32_t app_version,
	const std::vector<std::string>& layers,
	const std::vector<std::string>& extensions)
{
	vk::ApplicationInfo app_info(
		app_name.c_str(),
		app_version,
		"Gorilla Engine",
		VK_MAKE_VERSION(0, 1, 0),
		VK_API_VERSION_1_3);

	std::vector<const char*> layers_raw;
	layers_raw.reserve(layers.size());
	std::vector<const char*> extensions_raw;
	extensions_raw.reserve(extensions.size());

	std::ranges::transform(layers, std::back_inserter(layers_raw), &std::string::c_str);
	std::ranges::transform(extensions, std::back_inserter(extensions_raw), &std::string::c_str);

	vk::InstanceCreateInfo instance_create_info(
		{ },
		&app_info,
		layers_raw,
		extensions_raw);
	instance_ = std::make_shared<vk::raii::Instance>(vk::raii::Context{ }, instance_create_info);
}

std::vector<physical_device> instance::enumerate_physical_devices() const
{
	std::vector<vk::raii::PhysicalDevice> physical_devices = instance_->enumeratePhysicalDevices();
	std::vector<physical_device> result;
	result.reserve(physical_devices.size());
	
	std::ranges::transform(
		physical_devices,
		std::back_inserter(result),
		[this](const vk::raii::PhysicalDevice& device)
		{
			return physical_device{ std::make_shared<vk::raii::PhysicalDevice>(*this, *device) };
		});

	return result;
}

}