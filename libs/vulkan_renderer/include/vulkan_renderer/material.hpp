#ifndef GORILLA_MATERIAL_HPP
#define GORILLA_MATERIAL_HPP

#include <vulkan/vulkan_raii.hpp>

#include <vulkan_renderer/environment.hpp>


namespace gorilla::vulkan_renderer
{

class material
{
public:
	material(
		const environment& env,
		const std::vector<vk::PipelineShaderStageCreateInfo>& shader_stages,
		const vk::PipelineVertexInputStateCreateInfo& vertex_input_info,
		const vk::PipelineLayout& pipeline_layout,
		const vk::RenderPass& render_pass);

private:
	vk::raii::Pipeline pipeline_ = nullptr;
};

}
#endif //GORILLA_MATERIAL_HPP
