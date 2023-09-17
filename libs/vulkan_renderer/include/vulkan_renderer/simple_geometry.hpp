#ifndef GORILLA_SIMPLE_GEOMETRY_HPP
#define GORILLA_SIMPLE_GEOMETRY_HPP

#include <vulkan/vulkan_raii.hpp>

#include <glm/glm.hpp>

#include <vulkan_renderer/drawable.hpp>
#include <vulkan_renderer/aos_mesh_primitive.hpp>


namespace gorilla::vulkan_renderer
{

class simple_geometry : public drawable
{
public:
	simple_geometry() = default;

	simple_geometry(
		const environment& env,
		const std::vector<glm::vec3>& vertices,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec3>& colors,
		const std::vector<uint32_t>& indices);

	void draw(const vk::raii::CommandBuffer& cmd) const override;

private:
	aos_mesh_primitive aos_mesh_;

	glm::mat4 transform_ = glm::mat4(1.0f);
	vk::raii::Buffer transform_buffer_ = nullptr;
	vk::raii::DeviceMemory transform_buffer_memory_ = nullptr;
};

}

#endif //GORILLA_SIMPLE_GEOMETRY_HPP
