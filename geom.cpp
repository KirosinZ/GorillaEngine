#include "geom.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>


namespace gorilla
{

geom::geom(
	std::vector<glm::vec3> v,
	std::vector<glm::vec3> vn,
	std::vector<glm::vec3> vt,
	std::vector<int32_t> polygon_indices,
	std::vector<uint32_t> polygon_offsets)
	: v_(std::move(v)),
	  vn_(std::move(vn)),
	  vt_(std::move(vt)),
	  polygon_indices_(std::move(polygon_indices)),
	  polygon_offsets_(std::move(polygon_offsets))
{}

geom geom::parse(const std::string& filename, std::string& err_msg)
{
	std::ifstream file(filename, std::ios::in);

	std::vector<glm::vec3> vertices_read;
	std::vector<glm::vec3> normals_read;
	std::vector<glm::vec3> texture_coords_read;

	struct index_triplet
	{
		int32_t v_i = 0;
		int32_t vn_i = -1;
		int32_t vt_i = -1;

		bool operator==(const index_triplet& other) const
		{
			return std::tie(v_i, vn_i, vt_i) == std::tie(other.v_i, other.vn_i, other.vt_i);
		}
	};

	struct it_hash
	{
		std::size_t operator()(const index_triplet& triplet) const
		{
			return std::hash<int32_t>()(triplet.v_i)
			       ^ std::hash<int32_t>()(triplet.vn_i)
			       ^ std::hash<int32_t>()(triplet.vt_i);
		}
	};

	std::unordered_map<index_triplet, uint32_t, it_hash> unique_index_triples;

	std::vector<glm::vec3> actual_vertices;
	std::vector<glm::vec3> actual_normals;
	std::vector<glm::vec3> actual_texture_coords;

	std::vector<int32_t> polygon_indices;
	std::vector<uint32_t> polygon_offsets;

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
			      || token == "vt"
			      || token == "f"))
				break;

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
				vertices_read.push_back(vertex);
				continue;
			}
			else if (token == "vn")
			{
				glm::vec3 normal;
				line_stream >> normal.x >> normal.y >> normal.z;
				normals_read.push_back(normal);
				continue;
			}
			else if (token == "vt")
			{
				glm::vec3 texture_coords;
				line_stream >> texture_coords.x >> texture_coords.y;
				if (!line_stream.eof())
					line_stream >> texture_coords.z;
				texture_coords_read.push_back(texture_coords);
				continue;
			}
			else if (token == "f")
			{
				int n_vertices = 0;
				while (!line_stream.eof())
				{
					std::string indices;
					line_stream >> indices;

					std::stringstream index_stream(indices);

					index_triplet it{ };
					index_stream >> it.v_i;

					if (!index_stream.eof())
					{
						char c;
						index_stream.get(c);
						index_stream.get(c);
						if (c != '/')
						{
							index_stream.unget();
							index_stream >> it.vt_i;
							if (!index_stream.eof())
							{
								index_stream.get(c);
								index_stream >> it.vn_i;
							}
						}
						else
						{
							index_stream >> it.vn_i;
						}
					}

					bool inserted = false;
					std::tie(std::ignore, inserted) = unique_index_triples.insert({ it, unique_index_triples.size() });

					if (inserted)
					{
						actual_vertices.push_back(vertices_read[it.v_i - 1]);
						if (it.vt_i != -1)
							actual_texture_coords.push_back(texture_coords_read[it.vt_i - 1]);
						if (it.vn_i != -1)
							actual_normals.push_back(normals_read[it.vn_i - 1]);
					}

					polygon_indices.push_back(unique_index_triples[it]);
					n_vertices++;
					if (polygon_offsets.empty())
						polygon_offsets.push_back(0);
				}
				polygon_offsets.push_back(polygon_offsets.back() + n_vertices);
			}
		}
	}

	return { actual_vertices, actual_normals, actual_texture_coords, polygon_indices, polygon_offsets };
}

} // gorilla