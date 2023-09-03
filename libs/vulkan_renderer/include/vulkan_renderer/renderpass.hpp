#ifndef GORILLA_RENDERPASS_HPP
#define GORILLA_RENDERPASS_HPP

#include <vulkan/vulkan_raii.hpp>

#include <error_handling/error_handling.hpp>

#include "environment.hpp"


namespace gorilla::vulkan_renderer
{

struct render_pass_create_info
{

};

class render_pass
{
public:

	render_pass(const render_pass&) = delete;

	render_pass& operator=(const render_pass&) = delete;

	render_pass(render_pass&&) noexcept = default;

	render_pass& operator=(render_pass&&) noexcept = default;

	~render_pass() noexcept = default;

	static error_handling::result<render_pass> ctor(
		const environment& env,
		const vk::Format& image_format,
		const vk::Format& depth_format,
		const render_pass_create_info& info = { });

	[[nodiscard]] inline const vk::raii::RenderPass& vk_render_pass() const noexcept
	{ return m_render_pass; }

	[[nodiscard]] inline const std::vector<vk::AttachmentDescription>& attachments() const noexcept
	{ return m_attachments; }

private:
	render_pass() noexcept = default;

	vk::raii::RenderPass m_render_pass = nullptr;
	std::vector<vk::AttachmentDescription> m_attachments{ };
};

} // vulkan_renderer

#endif //GORILLA_RENDERPASS_HPP
