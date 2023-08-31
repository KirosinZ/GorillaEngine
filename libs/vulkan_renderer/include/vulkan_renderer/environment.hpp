#ifndef GORILLA_ENVIRONMENT_HPP
#define GORILLA_ENVIRONMENT_HPP

#include <vulkan/vulkan_raii.hpp>

#include <error_handling/exceptions.hpp>


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

	environment() = delete;

	environment(const environment&) = delete;

	environment& operator=(const environment&) = delete;

	environment(environment&&) noexcept = default;

	environment& operator=(environment&&) noexcept = default;

	~environment() = default;

	static gorilla::error_handling::result<environment> ctor(const environment_create_info& create_info = { });

	[[nodiscard]] inline const vk::raii::Context& context() const noexcept
	{ return m_context; }

	[[nodiscard]] inline const vk::raii::Instance& instance() const noexcept
	{ return m_instance; }

	[[nodiscard]] inline const vk::raii::PhysicalDevice& phys_device() const noexcept
	{ return m_phys_device; }

	[[nodiscard]] inline const vk::raii::Device& device() const noexcept
	{ return m_device; }

	[[nodiscard]] inline int graphics_family() const noexcept
	{ return m_graphics_family; }

	[[nodiscard]] inline const vk::raii::Queue& graphics_queue() const noexcept
	{ return m_queues[0]; }

	[[nodiscard]] inline int compute_family() const noexcept
	{ return m_compute_family; }

	[[nodiscard]] inline const vk::raii::Queue& compute_queue() const noexcept
	{ return m_queues[1]; }

	[[nodiscard]] inline int transfer_family() const noexcept
	{ return m_transfer_family; }

	[[nodiscard]] inline const vk::raii::Queue& transfer_queue() const noexcept
	{ return m_queues[2]; }

private:
	environment(
		vk::raii::Context t_context,
		vk::raii::Instance t_instance,
		vk::raii::PhysicalDevice t_phys_device,
		vk::raii::Device t_device,
		int t_graphics_family,
		int t_compute_family,
		int t_transfer_family,
		std::vector<vk::raii::Queue> t_queues) noexcept;

	vk::raii::Context m_context;
	vk::raii::Instance m_instance = nullptr;
	vk::raii::PhysicalDevice m_phys_device = nullptr;
	vk::raii::Device m_device = nullptr;
	int m_graphics_family = -1;
	int m_compute_family = -1;
	int m_transfer_family = -1;
	std::vector<vk::raii::Queue> m_queues;
};

}

#endif //GORILLA_ENVIRONMENT_HPP
