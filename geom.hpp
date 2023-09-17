#ifndef GORILLA_GEOM_HPP
#define GORILLA_GEOM_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>


namespace gorilla
{

class geom
{
public:
	geom() = default;

	geom(
		std::vector<glm::vec3> v,
		std::vector<glm::vec3> vn,
		std::vector<glm::vec3> vt,
		std::vector<int32_t> polygon_indices,
		std::vector<uint32_t> polygon_offsets);

	static geom parse(const std::string& filename, std::string& err_msg);

	inline const std::vector<glm::vec3>& vertices() const
	{ return v_; }

	inline const size_t n_vertices() const
	{ return v_.size(); }

	inline const std::vector<glm::vec3>& normals() const
	{ return vn_; }

	inline const std::vector<glm::vec3>& texture_coords() const
	{ return vt_; }

	inline const std::vector<int32_t>& polygon_indices() const
	{ return polygon_indices_; }

	inline const std::vector<uint32_t>& polygon_offsets() const
	{ return polygon_offsets_; }

	inline const size_t n_polygons() const
	{ return std::max(0ull, polygon_offsets_.size() - 1); }

private:
	std::vector<glm::vec3> v_{ };
	std::vector<glm::vec3> vn_{ };
	std::vector<glm::vec3> vt_{ };

	std::vector<int32_t> polygon_indices_{ };
	std::vector<uint32_t> polygon_offsets_{ };
};

} // gorilla

#endif //GORILLA_GEOM_HPP
