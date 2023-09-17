#include <vulkan_renderer/material.hpp>


namespace gorilla::vulkan_renderer
{

material::material(
	const environment& env,
	const std::vector<vk::PipelineShaderStageCreateInfo>& shader_stages,
	const vk::PipelineVertexInputStateCreateInfo& vertex_input_info,
	const vk::PipelineLayout& pipeline_layout,
	const vk::RenderPass& render_pass)
{
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
		{ },
		vk::PrimitiveTopology::eTriangleList,
		false);

	vk::PipelineTessellationStateCreateInfo tessellation_info{ };
	vk::PipelineViewportStateCreateInfo viewport_state_info{ };

	vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info(
		{ },
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eCounterClockwise,
		false,
		0.0f,
		0.0f,
		0.0f,
		1.0f);

	vk::PipelineMultisampleStateCreateInfo multisample_state_create_info(
		{ },
		vk::SampleCountFlagBits::e1,
		false);

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info(
		{ },
		true,
		true,
		vk::CompareOp::eLess,
		false,
		false);

	vk::PipelineColorBlendAttachmentState color_blend_attachment_state(
		false);
	color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR
	                                              | vk::ColorComponentFlagBits::eG
	                                              | vk::ColorComponentFlagBits::eB
	                                              | vk::ColorComponentFlagBits::eA;
	vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info(
		{ },
		false,
		vk::LogicOp::eCopy,
		color_blend_attachment_state,
		{ 0.0f, 0.0f, 0.0f, 0.0f });

	std::vector<vk::DynamicState> dynamic_states{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	vk::PipelineDynamicStateCreateInfo dynamic_state_create_info(
		{ },
		dynamic_states);

	vk::GraphicsPipelineCreateInfo pipeline_create_info(
		{ },
		shader_stages,
		&vertex_input_info,
		&input_assembly_info,
		&tessellation_info,
		&viewport_state_info,
		&rasterization_state_create_info,
		&multisample_state_create_info,
		&depth_stencil_state_create_info,
		&color_blend_state_create_info,
		&dynamic_state_create_info,
		pipeline_layout,
		render_pass,
		0);
	pipeline_ = std::move(env.device().createGraphicsPipelines(nullptr, pipeline_create_info)[0]);
}

}