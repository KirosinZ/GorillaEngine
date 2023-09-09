#ifndef GORILLA_SHADER_HPP
#define GORILLA_SHADER_HPP

#include <vulkan_renderer/environment.hpp>


namespace gorilla::vulkan_renderer
{

class shader
{
public:
	enum class kind
	{
		vertex,
		fragment
	};
public:
	shader() = default;

	shader(std::shared_ptr<vk::raii::ShaderModule> module, kind kind);

	shader(const environment& env, const std::vector<uint32_t>& code, kind kind);

	static shader from_file(const environment& env, const std::string& filename, std::string& err_msg);

	vk::PipelineShaderStageCreateInfo stage_create_info() const;

private:
	std::shared_ptr<vk::raii::ShaderModule> module_ = nullptr;

	kind kind_{ };
};

}

#endif //GORILLA_SHADER_HPP
