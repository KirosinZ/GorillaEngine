#ifndef GORILLA_GEOM_HPP
#define GORILLA_GEOM_HPP

#include <vector>
#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>


namespace gorilla
{

class geom
{
public:
	geom() = default;

	geom(std::vector<glm::vec3> v, std::vector<glm::vec3> vn, std::vector<int32_t> triangle_indices);

	static geom parse(const std::string& filename, std::string& err_msg);

	inline const std::vector<glm::vec3>& vertices() const
	{ return v_; }

	inline std::vector<glm::vec3>& vertices()
	{ return v_; }

	inline const std::vector<glm::vec3>& normals() const
	{ return vn_; }

	inline std::vector<glm::vec3>& normals()
	{ return vn_; }

	inline const std::vector<int32_t>& triangle_indices() const
	{ return triangle_indices_; }

	inline std::vector<int32_t>& triangle_indices()
	{ return triangle_indices_; }

	std::vector<vk::VertexInputBindingDescription> binding_descriptions() const;

	std::vector<vk::VertexInputAttributeDescription> attribute_descriptions() const;

private:
	std::vector<glm::vec3> v_{ };
	std::vector<glm::vec3> vn_{ };

	std::vector<int32_t> triangle_indices_{ };
};

} // gorilla

#endif //GORILLA_GEOM_HPP
