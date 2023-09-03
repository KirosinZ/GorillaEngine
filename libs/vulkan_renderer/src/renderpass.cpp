#include "vulkan_renderer/renderpass.hpp"


namespace gorilla::vulkan_renderer
{

error_handling::result<render_pass> render_pass::ctor(const environment& env, const vk::Format& image_format, const vk::Format& depth_format, const render_pass_create_info& info)
{
	const vk::raii::Device& device = env.device();

	render_pass res{ };

	vk::AttachmentDescription color_attachment(
		{ },
		image_format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentDescription depth_attachment(
		{ },
		depth_format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference color_ref(
		0,
		vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentReference depth_ref(
		1,
		vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass(
		{ },
		vk::PipelineBindPoint::eGraphics,
		{ },
		color_ref,
		{ },
		&depth_ref,
		{ });

	res.m_attachments = {
		color_attachment,
		depth_attachment,
	};

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		{ },
		vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	vk::RenderPassCreateInfo render_pass_create_info(
		{ },
		res.m_attachments,
		subpass,
		dependency);
	res.m_render_pass = device.createRenderPass(render_pass_create_info);

	return res;
}

} // vulkan_renderer