#include "geom.hpp"

#include <fstream>
#include <sstream>


namespace gorilla
{

geom::geom(std::vector<glm::vec3> v, std::vector<glm::vec3> vn, std::vector<int32_t> triangle_indices)
	: v_(std::move(v)),
	  vn_(std::move(vn)),
	  triangle_indices_(std::move(triangle_indices))
{}

geom geom::parse(const std::string& filename, std::string& err_msg)
{
	std::ifstream file(filename, std::ios::in | std::ios::ate);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<int32_t> triangle_indices;

	if (!file.is_open())
	{
		err_msg = "couldn't open file \"" + filename + "\"";
		return { };
	}

	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		std::stringstream line_stream(line);

		while (!line_stream.eof())
		{
			std::string token;
			line_stream >> token;

			if (!(token == "v"
			      || token == "vn"
			      || token == "f"))
				continue;

			if (token == "v")
			{
				glm::vec3 vertex;
				line_stream >> vertex.x >> vertex.y >> vertex.z;
				if (!line_stream.eof())
				{
					float w = 1.0f;
					line_stream >> w;
					vertex /= w;
				}
				vertices.push_back(vertex);
				continue;
			}
			else if (token == "vn")
			{
				glm::vec3 normal;
				line_stream >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
				continue;
			}
			else if (token == "f")
			{
				std::string indices;
				line_stream >> indices;
			}
		}
	}
}

std::vector<vk::VertexInputBindingDescription> geom::binding_descriptions() const
{
	return {
		vk::VertexInputBindingDescription(0, sizeof(glm::vec3), vk::VertexInputRate::eVertex),
		vk::VertexInputBindingDescription(1, sizeof(glm::vec3), vk::VertexInputRate::eVertex),
	};
}

std::vector<vk::VertexInputAttributeDescription> geom::attribute_descriptions() const
{
	return {
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0),
		vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32B32Sfloat, 0),
	};
}

} // gorilla