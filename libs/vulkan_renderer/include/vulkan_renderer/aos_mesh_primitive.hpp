#ifndef GORILLA_AOS_MESH_PRIMITIVE_HPP
#define GORILLA_AOS_MESH_PRIMITIVE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <glm/glm.hpp>

#include <vulkan_renderer/primitive.hpp>
#include <vulkan_renderer/environment.hpp>


namespace gorilla::vulkan_renderer
{

class aos_mesh_primitive : public primitive
{
	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
	};
public:
	aos_mesh_primitive() = default;

	aos_mesh_primitive(
		const environment& env,
		const std::vector<glm::vec3>& vertices,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec3>& colors,
		const std::vector<uint32_t>& indices);

	std::vector<vk::VertexInputBindingDescription> binding_descriptions() const override;

	std::vector<vk::VertexInputAttributeDescription> attribute_descriptions() const override;

	inline std::pair<vk::PrimitiveTopology, bool> topology() const override;

	void draw(const vk::raii::CommandBuffer& cmd) const override;

private:
	std::vector<vertex> vertices_;
	std::vector<uint32_t> indices_;

	vk::raii::Buffer vertex_buffer_ = nullptr;
	vk::raii::DeviceMemory vertex_buffer_memory_ = nullptr;

	vk::raii::Buffer index_buffer_ = nullptr;
	vk::raii::DeviceMemory index_buffer_memory_ = nullptr;
};

}

#endif //GORILLA_AOS_MESH_PRIMITIVE_HPP
