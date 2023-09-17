#ifndef GORILLA_ENVIRONMENT_HPP
#define GORILLA_ENVIRONMENT_HPP

#include <vulkan/vulkan_raii.hpp>

#include <error_handling/exceptions.hpp>

#include <vulkan_renderer/device_memory_manager.hpp>


namespace gorilla::vulkan_renderer
{

struct environment_create_info
{
	std::string app_name{ };
	uint32_t app_version = VK_MAKE_VERSION(0, 0, 1);

	std::vector<const char*> desired_instance_layers{ };
	std::vector<const char*> desired_instance_extensions{ };

	std::vector<const char*> desired_device_extensions{ };

	vk::PhysicalDeviceFeatures desired_features{ };
};

class environment
{
public:
	environment(const environment_create_info& create_info = { });

	environment(const environment&) = delete;

	environment& operator=(const environment&) = delete;

	environment(environment&&) noexcept = default;

	environment& operator=(environment&&) noexcept = default;

	[[nodiscard]] inline const vk::raii::Context& context() const noexcept
	{ return context_; }

	[[nodiscard]] inline const vk::raii::Instance& instance() const noexcept
	{ return instance_; }

	[[nodiscard]] inline const vk::raii::PhysicalDevice& phys_device() const noexcept
	{ return phys_device_; }

	[[nodiscard]] inline const vk::raii::Device& device() const noexcept
	{ return device_; }

	[[nodiscard]] inline int graphics_family() const noexcept
	{ return graphics_family_; }

	[[nodiscard]] inline const vk::raii::Queue& graphics_queue() const noexcept
	{ return queues_[0]; }

	[[nodiscard]] inline int compute_family() const noexcept
	{ return compute_family_; }

	[[nodiscard]] inline const vk::raii::Queue& compute_queue() const noexcept
	{ return queues_[1]; }

	[[nodiscard]] inline int transfer_family() const noexcept
	{ return transfer_family_; }

	[[nodiscard]] inline const vk::raii::Queue& transfer_queue() const noexcept
	{ return queues_[2]; }

	[[nodiscard]] inline const device_memory_manager& memory_manager() const noexcept
	{ return memory_manager_; }

private:
	vk::raii::Context context_;
	vk::raii::Instance instance_ = nullptr;
	vk::raii::PhysicalDevice phys_device_ = nullptr;
	vk::raii::Device device_ = nullptr;
	int graphics_family_ = -1;
	int compute_family_ = -1;
	int transfer_family_ = -1;
	std::vector<vk::raii::Queue> queues_;

	device_memory_manager memory_manager_;
};

}

#endif //GORILLA_ENVIRONMENT_HPP
