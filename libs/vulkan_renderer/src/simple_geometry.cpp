#include <vulkan_renderer/simple_geometry.hpp>


namespace gorilla::vulkan_renderer
{

simple_geometry::simple_geometry(
	const environment& env,
	const std::vector<glm::vec3>& vertices,
	const std::vector<glm::vec3>& normals,
	const std::vector<glm::vec3>& colors,
	const std::vector<uint32_t>& indices)
	: aos_mesh_(env, vertices, normals, colors, indices)
{
	const auto bindings = aos_mesh_.binding_descriptions();
	const auto attributes = aos_mesh_.attribute_descriptions();
	vk::PipelineVertexInputStateCreateInfo vertex_input_info(
		{ },
		bindings,
		attributes);

	const auto [topology, restart_enable] = aos_mesh_.topology();
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
		{ },
		topology,
		restart_enable);

	vk::PipelineTessellationStateCreateInfo tessellation_info{ };

	vk::PipelineViewportStateCreateInfo viewport_state_info{ };

	vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info(
		{ },
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eNone,
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
		vk::DynamicState::eScissor,
	};
	vk::PipelineDynamicStateCreateInfo dynamic_state_create_info(
		{ },
		dynamic_states);

	vk::DescriptorSetLayoutBinding descriptor_set_layout_binding(
		0,
		vk::DescriptorType::eUniformBuffer,
		1,
		vk::ShaderStageFlagBits::eVertex);
	vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(
		{ },
		descriptor_set_layout_binding);
	descriptor_set_layout_ = env_->device().createDescriptorSetLayout(descriptor_set_layout_create_info);

	vk::PipelineLayoutCreateInfo pipeline_layout_create_info(
		{ },
		*descriptor_set_layout_);
	pipeline_layout_ = env_->device().createPipelineLayout(pipeline_layout_create_info);

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
		*pipeline_layout_,
		*render_pass_,
		0);
	pipeline_ = std::move(env.device().createGraphicsPipelines(nullptr, pipeline_create_info)[0]);
}

}