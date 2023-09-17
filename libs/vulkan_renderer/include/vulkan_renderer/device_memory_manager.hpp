#ifndef GORILLA_DEVICE_MEMORY_MANAGER_HPP
#define GORILLA_DEVICE_MEMORY_MANAGER_HPP

#include <ranges>

#include <vulkan/vulkan_raii.hpp>


namespace gorilla::vulkan_renderer
{

class environment;

class device_memory_manager
{
public:
	device_memory_manager() = default;

	device_memory_manager(const environment& env);

	vk::raii::DeviceMemory allocate(const size_t size, const vk::MemoryPropertyFlags& properties, const size_t memory_type_mask = 0xFFFFFFFF) const;

	vk::raii::DeviceMemory allocate(const vk::raii::Buffer& buffer, const vk::MemoryPropertyFlags& properties) const;


	void copy(const vk::raii::Buffer& src, const size_t src_offset, const vk::raii::Buffer& dst, const int dst_offset, const size_t size) const;

	void copy(const void* data, const vk::raii::Buffer& dst, const int dst_offset, const size_t size) const;

private:
	const environment* env_ = nullptr;

	vk::raii::CommandPool copy_cmd_pool_ = nullptr;

	vk::raii::Buffer staging_buffer_ = nullptr;
	vk::raii::DeviceMemory staging_buffer_memory_ = nullptr;
};

}

#endif //GORILLA_DEVICE_MEMORY_MANAGER_HPP
