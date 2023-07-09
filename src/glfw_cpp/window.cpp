#include "window.hpp"

void set_window_hints(const glfw::window_init_info& init_info)
{
	glfwWindowHint(GLFW_RED_BITS, init_info.framebuffer_hints.red_bits);
	glfwWindowHint(GLFW_GREEN_BITS, init_info.framebuffer_hints.green_bits);
	glfwWindowHint(GLFW_BLUE_BITS, init_info.framebuffer_hints.blue_bits);
	glfwWindowHint(GLFW_ALPHA_BITS, init_info.framebuffer_hints.alpha_bits);
	glfwWindowHint(GLFW_DEPTH_BITS, init_info.framebuffer_hints.depth_bits);
	glfwWindowHint(GLFW_STENCIL_BITS, init_info.framebuffer_hints.stencil_bits);
	glfwWindowHint(GLFW_STEREO, init_info.framebuffer_hints.stereo);
	glfwWindowHint(GLFW_DOUBLEBUFFER, init_info.framebuffer_hints.doublebuffer);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, init_info.framebuffer_hints.transparent);
	glfwWindowHint(GLFW_SAMPLES, init_info.framebuffer_hints.samples);
	glfwWindowHint(GLFW_SRGB_CAPABLE, init_info.framebuffer_hints.srgb_capable);

	glfwWindowHint(GLFW_RESIZABLE, init_info.window_hints.resizable);
	glfwWindowHint(GLFW_DECORATED, init_info.window_hints.decorated);
	glfwWindowHint(GLFW_FOCUSED, init_info.window_hints.focused);
	glfwWindowHint(GLFW_AUTO_ICONIFY, init_info.window_hints.minimized);
	glfwWindowHint(GLFW_FLOATING, init_info.window_hints.top_most);
	glfwWindowHint(GLFW_MAXIMIZED, init_info.window_hints.maximized);
	glfwWindowHint(GLFW_VISIBLE, init_info.window_hints.visible);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, init_info.window_hints.scale_to_monitor);
	glfwWindowHint(GLFW_CENTER_CURSOR, init_info.window_hints.center_cursor);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, init_info.window_hints.focus_on_show);

	glfwWindowHint(GLFW_CLIENT_API, static_cast<i32>(init_info.context_hints.client));
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, static_cast<i32>(init_info.context_hints.source));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, init_info.context_hints.version_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, init_info.context_hints.version_minor);
	glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, static_cast<i32>(init_info.context_hints.robustness));
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, init_info.context_hints.opengl_fwd_compat);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, init_info.context_hints.opengl_debug_context);
	glfwWindowHint(GLFW_NO_ERROR, init_info.context_hints.noerror);
	glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<i32>(init_info.context_hints.opengl_profile));
	glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, static_cast<i32>(init_info.context_hints.release));
	glfwWindowHint(GLFW_REFRESH_RATE, init_info.refresh_rate);
}

namespace glfw
{

window::window(
		i32 width,
		i32  height,
		const std::string& title,
		const glfw::window_init_info& init_info)
	: m_width(width),
	  m_height(height),
	  m_title(title)
{
	set_window_hints(init_info);
	m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

window::window(
		i32 width,
		i32 height,
		const std::string& title,
		const glfw::monitor& mnt,
		const glfw::window_init_info& init_info)
	: m_width(width),
	  m_height(height),
	  m_title(title)
{
	set_window_hints(init_info);
	m_handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), nullptr);
}

window::window(
		i32 width,
		i32 height,
		const std::string& title,
		const glfw::window& share,
		const glfw::window_init_info& init_info)
	: m_width(width),
	  m_height(height),
	  m_title(title)
{
	set_window_hints(init_info);
	m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, share.handle());
}

window::window(
		i32 width,
		i32 height,
		const std::string& title,
		const glfw::monitor& mnt,
		const glfw::window& share,
		const glfw::window_init_info& init_info)
		: m_width(width),
		  m_height(height),
		  m_title(title)
{
	set_window_hints(init_info);
	m_handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), share.handle());
}

window::window(glfw::window&& move) noexcept
	: m_width(move.m_width),
	  m_height(move.m_height),
	  m_title(std::move(move.m_title)),
	  m_handle(std::exchange(move.m_handle, nullptr))
{}

window& window::operator=(glfw::window&& move) noexcept
{
	if (this == &move)
		return *this;

	m_width = move.m_width;
	m_height = move.m_height;
	m_title = std::move(move.m_title);

	glfwDestroyWindow(m_handle);
	m_handle = std::exchange(move.m_handle, nullptr);

	return *this;
}

window::~window() noexcept
{
	glfwDestroyWindow(m_handle);
}


bool window::focused() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_FOCUSED);
}

bool window::minimized() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_ICONIFIED);
}

bool window::maximized() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_MAXIMIZED);
}

bool window::visible() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_VISIBLE);
}

bool window::resizable() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_RESIZABLE);
}

bool window::decorated() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_DECORATED);
}

bool window::top_most() const
{
	return glfwGetWindowAttrib(m_handle, GLFW_FLOATING);
}


void minimize();
void maximize();
void show();
void hide();

}