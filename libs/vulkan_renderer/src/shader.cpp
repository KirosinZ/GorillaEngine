#include <vulkan_renderer/shader.hpp>

#include <fstream>
#include <filesystem>


namespace gorilla::vulkan_renderer
{

shader::shader(std::shared_ptr<vk::raii::ShaderModule> module, kind kind)
	: module_(module),
	  kind_(kind)
{}

shader::shader(const environment& env, const std::vector<uint32_t>& code, kind kind)
	: kind_(kind)
{
	vk::ShaderModuleCreateInfo create_info(
		{ },
		code);
	module_ = std::make_shared<vk::raii::ShaderModule>(env.device(), create_info);
}

shader shader::from_file(const environment& env, const std::string& filename, std::string& err_msg)
{
	std::filesystem::path path(filename);

	std::string ext = path.extension().string();
	if (ext.compare(".spv"))
	{
		err_msg = "Compiled shaders must have .spv extension";
		return { };
	}

	ext = path.stem().extension().string();
	kind kind;
	if (!ext.compare(".vert"))
		kind = kind::vertex;
	else if (!ext.compare(".frag"))
		kind = kind::fragment;
	else
	{
		err_msg = "Unknown shader file extension";
		return { };
	}
	std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		err_msg = "couldn't open file \"" + filename + "\"";
		return { };
	}

	int size = file.tellg();
	std::vector<uint32_t> buffer(size / sizeof(uint32_t));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), size);
	file.close();

	return { env, buffer, kind };
}

vk::PipelineShaderStageCreateInfo shader::stage_create_info() const
{
	vk::ShaderStageFlagBits stage;
	switch (kind_)
	{
		case kind::vertex:
			stage = vk::ShaderStageFlagBits::eVertex;
			break;
		case kind::fragment:
			stage = vk::ShaderStageFlagBits::eFragment;
			break;
		default:
			return { };
	}
	return vk::PipelineShaderStageCreateInfo(
		{ },
		stage,
		*(*module_),
		"main");
}

}