#include <vulkan_renderer/device_memory_manager.hpp>

#include <vulkan_renderer/environment.hpp>


namespace gorilla::vulkan_renderer
{

device_memory_manager::device_memory_manager(const environment& env)
	: env_{ &env }
{
	vk::CommandPoolCreateInfo cmd_pool_info(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		env.graphics_family());
	copy_cmd_pool_ = env_->device().createCommandPool(cmd_pool_info);

	std::vector<uint32_t> qfs = {
		uint32_t(env.graphics_family()),
	};
	vk::BufferCreateInfo buffer_info(
		{ },
		1024 * 1024 * 10,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		qfs);
	staging_buffer_ = env_->device().createBuffer(buffer_info);
	staging_buffer_memory_ = allocate(
		staging_buffer_,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	vk::BindBufferMemoryInfo bind_info(
		*staging_buffer_,
		*staging_buffer_memory_,
		0);
	env_->device().bindBufferMemory2(bind_info);
}

vk::raii::DeviceMemory device_memory_manager::allocate(const size_t size, const vk::MemoryPropertyFlags& properties, const size_t memory_type_mask) const
{
	const vk::PhysicalDeviceMemoryProperties props = env_->phys_device().getMemoryProperties();
	int memory_type_index = -1;
	for (int mti = 0; mti < props.memoryTypeCount; mti++)
	{
		if ((memory_type_mask & (1 << mti))
		    && (props.memoryTypes[mti].propertyFlags & properties) == properties)
		{
			memory_type_index = mti;
			break;
		}
	}

	vk::MemoryAllocateInfo alloc_info(
		size,
		memory_type_index);

	return env_->device().allocateMemory(alloc_info);
}

vk::raii::DeviceMemory device_memory_manager::allocate(const vk::raii::Buffer& buffer, const vk::MemoryPropertyFlags& properties) const
{
	vk::BufferMemoryRequirementsInfo2 reqs(*buffer);
	vk::MemoryRequirements2 memory_requirements = env_->device().getBufferMemoryRequirements2(reqs);

	return allocate(
		memory_requirements.memoryRequirements.size,
		properties,
		memory_requirements.memoryRequirements.memoryTypeBits);
}

void device_memory_manager::copy(const void* data, const vk::raii::Buffer& dst, const int dst_offset, const size_t size) const
{
	void* map = staging_buffer_memory_.mapMemory(0, size);
	std::memcpy(map, data, size);
	staging_buffer_memory_.unmapMemory();

	vk::CommandBufferAllocateInfo cmd_alloc_info(
		*copy_cmd_pool_,
		vk::CommandBufferLevel::ePrimary,
		1);
	vk::raii::CommandBuffer cmd = std::move(env_->device().allocateCommandBuffers(cmd_alloc_info)[0]);

	vk::CommandBufferBeginInfo cmd_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(cmd_begin_info);
	vk::BufferCopy copy_region(
		0,
		dst_offset,
		size);
	cmd.copyBuffer(*staging_buffer_, *dst, copy_region);
	cmd.end();

	vk::SubmitInfo submit_info(
		{ },
		{ },
		*cmd,
		{ });
	env_->graphics_queue().submit(submit_info);
	env_->graphics_queue().waitIdle();
}

void device_memory_manager::copy(const vk::raii::Buffer& src, const size_t src_offset, const vk::raii::Buffer& dst, const int dst_offset, const size_t size) const
{
	vk::CommandBufferAllocateInfo cmd_alloc_info(
		*copy_cmd_pool_,
		vk::CommandBufferLevel::ePrimary,
		1);
	vk::raii::CommandBuffer cmd = std::move(env_->device().allocateCommandBuffers(cmd_alloc_info)[0]);

	vk::CommandBufferBeginInfo cmd_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(cmd_begin_info);
	vk::BufferCopy copy_region(
		src_offset,
		dst_offset,
		size);
	cmd.copyBuffer(*staging_buffer_, *dst, copy_region);
	cmd.end();

	vk::SubmitInfo submit_info(
		{ },
		{ },
		*cmd,
		{ });
	env_->graphics_queue().submit(submit_info);
	env_->graphics_queue().waitIdle();
}

}