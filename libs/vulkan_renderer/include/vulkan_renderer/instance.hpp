#ifndef GORILLA_INSTANCE_HPP
#define GORILLA_INSTANCE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <vulkan_renderer/physical_device.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

class instance
{
public:
	instance() = default;

	instance(std::shared_ptr<vk::raii::Instance> instance);

	instance(
		const std::string& app_name,
		const uint32_t app_version,
		const std::vector<std::string>& layers = { },
		const std::vector<std::string>& extensions = { });


	std::vector<physical_device> enumerate_physical_devices() const;

	inline operator const vk::raii::Instance&() const
	{ return *instance_; }

	inline const vk::Instance& operator*() const
	{ return *(*instance_); }

	inline const vk::raii::Instance* operator->() const
	{ return instance_.operator->(); }

private:
	std::shared_ptr<vk::raii::Instance> instance_ = nullptr;
};

}

#endif //GORILLA_INSTANCE_HPP
