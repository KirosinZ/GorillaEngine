#include <vulkan_renderer/aos_mesh_primitive.hpp>


namespace gorilla::vulkan_renderer
{

aos_mesh_primitive::aos_mesh_primitive(
	const gorilla::vulkan_renderer::environment& env,
	const std::vector<glm::vec3>& vertices,
	const std::vector<glm::vec3>& normals,
	const std::vector<glm::vec3>& colors,
	const std::vector<uint32_t>& indices)
	: indices_(indices)
{
	const size_t n_vertices = vertices.size();
	assert(normals.size() == n_vertices);

	vertices_.reserve(n_vertices);
	for (int v_ind = 0; v_ind < n_vertices; v_ind++)
	{
		vertices_.push_back({ vertices[v_ind], normals[v_ind], colors[v_ind] });
	}

	// TODO
	std::vector<uint32_t> gfs = {
		uint32_t(env.graphics_family()),
	};
	vk::BufferCreateInfo vertex_buffer_info(
		{ },
		n_vertices * sizeof(vertex),
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		gfs);
	vertex_buffer_ = env.device().createBuffer(vertex_buffer_info);
	vertex_buffer_memory_ = env.memory_manager().allocate(vertex_buffer_, vk::MemoryPropertyFlagBits::eDeviceLocal);

	vk::BufferCreateInfo index_buffer_info(
		{ },
		sizeof(uint32_t) * indices.size(),
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		gfs);
	index_buffer_ = env.device().createBuffer(index_buffer_info);
	index_buffer_memory_ = env.memory_manager().allocate(index_buffer_, vk::MemoryPropertyFlagBits::eDeviceLocal);

	std::vector<vk::BindBufferMemoryInfo> bind_infos = {
		vk::BindBufferMemoryInfo(
			*vertex_buffer_,
			*vertex_buffer_memory_,
			0),
		vk::BindBufferMemoryInfo(
			*index_buffer_,
			*index_buffer_memory_,
			0),
	};;
	env.device().bindBufferMemory2(bind_infos);

	env.memory_manager().copy(vertices_.data(), vertex_buffer_, 0, n_vertices * sizeof(vertex));
	env.memory_manager().copy(indices_.data(), index_buffer_, 0, indices.size() * sizeof(uint32_t));
}

std::vector<vk::VertexInputBindingDescription> aos_mesh_primitive::binding_descriptions() const
{
	return {
		vk::VertexInputBindingDescription(
			0,
			sizeof(vertex),
			vk::VertexInputRate::eVertex),
	};
}

std::vector<vk::VertexInputAttributeDescription> aos_mesh_primitive::attribute_descriptions() const
{
	return {
		vk::VertexInputAttributeDescription(
			0,
			0,
			vk::Format::eR32G32B32Sfloat,
			0),
		vk::VertexInputAttributeDescription(
			1,
			0,
			vk::Format::eR32G32B32Sfloat,
			offsetof(vertex, normal)),
		vk::VertexInputAttributeDescription(
			2,
			0,
			vk::Format::eR32G32B32Sfloat,
			offsetof(vertex, color)),
	};
}

std::pair<vk::PrimitiveTopology, bool> aos_mesh_primitive::topology() const
{
	return { vk::PrimitiveTopology::eTriangleList, false };
}

void aos_mesh_primitive::draw(const vk::raii::CommandBuffer& cmd) const
{
	cmd.bindVertexBuffers(0, *vertex_buffer_, { 0 });
	cmd.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);

	cmd.drawIndexed(indices_.size(), 1, 0, 0, 0);
}

}