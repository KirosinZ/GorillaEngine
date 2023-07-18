#include "window.hpp"

#include <error_handling/assert.hpp>
#include <utility>

err::result<void> set_window_hints(const glfw::window_init_info& init_info)
{
	const char* err;

	glfwWindowHint(GLFW_RED_BITS, init_info.framebuffer_hints.red_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_GREEN_BITS, init_info.framebuffer_hints.green_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_BLUE_BITS, init_info.framebuffer_hints.blue_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_ALPHA_BITS, init_info.framebuffer_hints.alpha_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_DEPTH_BITS, init_info.framebuffer_hints.depth_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_STENCIL_BITS, init_info.framebuffer_hints.stencil_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_STEREO, init_info.framebuffer_hints.stereo);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_DOUBLEBUFFER, init_info.framebuffer_hints.doublebuffer);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, init_info.framebuffer_hints.transparent);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_SAMPLES, init_info.framebuffer_hints.samples);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_SRGB_CAPABLE, init_info.framebuffer_hints.srgb_capable);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));


	glfwWindowHint(GLFW_RESIZABLE, init_info.window_hints.resizable);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_DECORATED, init_info.window_hints.decorated);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_FOCUSED, init_info.window_hints.focused);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_AUTO_ICONIFY, init_info.window_hints.minimized);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_FLOATING, init_info.window_hints.top_most);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_MAXIMIZED, init_info.window_hints.maximized);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_VISIBLE, init_info.window_hints.visible);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, init_info.window_hints.scale_to_monitor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CENTER_CURSOR, init_info.window_hints.center_cursor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, init_info.window_hints.focus_on_show);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));


	glfwWindowHint(GLFW_CLIENT_API, static_cast<i32>(init_info.context_hints.client));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_CREATION_API, static_cast<i32>(init_info.context_hints.source));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, init_info.context_hints.version_major);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, init_info.context_hints.version_minor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, static_cast<i32>(init_info.context_hints.robustness));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, init_info.context_hints.opengl_fwd_compat);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, init_info.context_hints.opengl_debug_context);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, init_info.context_hints.noerror);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<i32>(init_info.context_hints.opengl_profile));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, static_cast<i32>(init_info.context_hints.release));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	glfwWindowHint(GLFW_REFRESH_RATE, init_info.refresh_rate);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err));

	return {};
}

namespace glfw
{

window::window(std::string title, GLFWwindow* handle, std::any user_data) noexcept
	: m_title(std::move(title)),
	  m_handle(handle),
	  m_user_data(std::move(user_data))
{
	glfwSetWindowUserPointer(m_handle, this);
	glfwSetWindowPosCallback(m_handle, position_callback_raw);
	glfwSetWindowSizeCallback(m_handle, size_callback_raw);
	glfwSetWindowCloseCallback(m_handle, close_callback_raw);
	glfwSetWindowRefreshCallback(m_handle, refresh_callback_raw);
	glfwSetWindowFocusCallback(m_handle, focus_callback_raw);
	glfwSetWindowIconifyCallback(m_handle, minimize_callback_raw);
	glfwSetWindowMaximizeCallback(m_handle, maximize_callback_raw);
	glfwSetWindowContentScaleCallback(m_handle, contentscale_callback_raw);
	glfwSetFramebufferSizeCallback(m_handle, framebuffersize_callback_raw);
}

err::result<window> window::ctor(i32 width, i32 height, const std::string& title, const window_init_info& init_info)
{
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err_msg));

	return window(title, handle);
}

err::result<window> window::ctor(i32 width, i32 height, const std::string& title, const monitor& mnt, const window_init_info& init_info)
{
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), nullptr);
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err_msg));

	return window(title, handle);
}

err::result<window> window::ctor(i32 width, i32 height, const std::string& title, const window& share, const window_init_info& init_info)
{
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), nullptr, share.handle());
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err_msg));

	return window(title, handle);
}

err::result<window> window::ctor(i32 width, i32 height, const std::string& title, const monitor& mnt, const window& share, const window_init_info& init_info)
{
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), share.handle());
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return err::new_error(glfw::error{code}, std::string(err_msg));

	return window(title, handle);
}

window::window(glfw::window&& move) noexcept
		: m_title(std::move(move.m_title)),
		  m_handle(std::exchange(move.m_handle, nullptr)),
		  m_user_data(std::move(move.m_user_data)),
		  m_position_callback(std::move(move.m_position_callback)),
		  m_size_callback(std::move(move.m_size_callback)),
		  m_close_callback(std::move(move.m_close_callback)),
		  m_refresh_callback(std::move(move.m_refresh_callback)),
		  m_focus_callback(std::move(move.m_focus_callback)),
		  m_minimize_callback(std::move(move.m_minimize_callback)),
		  m_maximize_callback(std::move(move.m_maximize_callback)),
		  m_contentscale_callback(std::move(move.m_contentscale_callback)),
		  m_framebuffersize_callback(std::move(move.m_framebuffersize_callback))
{
	glfwSetWindowUserPointer(m_handle, this);
}

window& window::operator=(glfw::window&& move) noexcept
{
	if (this == &move)
		return *this;

	glfwDestroyWindow(m_handle);
	m_title                    = std::move(move.m_title);
	m_handle                   = std::exchange(move.m_handle, nullptr);
	m_user_data                = std::move(move.m_user_data);
	m_position_callback        = std::move(move.m_position_callback);
	m_size_callback            = std::move(move.m_size_callback);
	m_close_callback           = std::move(move.m_close_callback);
	m_refresh_callback         = std::move(move.m_refresh_callback);
	m_focus_callback           = std::move(move.m_focus_callback);
	m_minimize_callback        = std::move(move.m_minimize_callback);
	m_maximize_callback        = std::move(move.m_maximize_callback);
	m_contentscale_callback    = std::move(move.m_contentscale_callback);
	m_framebuffersize_callback = std::move(move.m_framebuffersize_callback);
	glfwSetWindowUserPointer(m_handle, this);

	return *this;
}

window::~window() noexcept
{
	glfwDestroyWindow(m_handle);
}


std::pair<int, int> window::position() const
{
	int xpos = 0;
	int ypos = 0;
	glfwGetWindowPos(m_handle, &xpos, &ypos);

	return { xpos, ypos };
}

void window::set_position(int xpos, int ypos)
{
	glfwSetWindowPos(m_handle, xpos, ypos);
}

std::pair<int, int> window::size() const
{
	int width  = 0;
	int height = 0;
	glfwGetWindowSize(m_handle, &width, &height);

	return { width, height };
}

void window::set_size(int width, int height)
{
	glfwSetWindowSize(m_handle, width, height);
}

void window::set_size_limits(int min_width, int min_height, int max_width, int max_height)
{
	glfwSetWindowSizeLimits(m_handle, min_width, min_height, max_width, max_height);
}

void window::set_aspect_ratio(int numer, int denom)
{
	glfwSetWindowAspectRatio(m_handle, numer, denom);
}

std::pair<int, int> window::framebuffer_size() const
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(m_handle, &width, &height);

	return { width, height };
}

std::pair<float, float> window::content_scale() const
{
	float xscale = 0.0f;
	float yscale = 0.0f;
	glfwGetWindowContentScale(m_handle, &xscale, &yscale);

	return { xscale, yscale };
}

float window::opacity() const
{
	return glfwGetWindowOpacity(m_handle);
}

void window::set_opacity(float value)
{
	glfwSetWindowOpacity(m_handle, value);
}

void window::minimize()
{
	glfwIconifyWindow(m_handle);
}

void window::restore()
{
	glfwRestoreWindow(m_handle);
}

void window::maximize()
{
	glfwMaximizeWindow(m_handle);
}

void window::show()
{
	glfwShowWindow(m_handle);
}

void window::hide()
{
	glfwHideWindow(m_handle);
}

void window::focus()
{
	glfwFocusWindow(m_handle);
}

void window::request_attention()
{
	glfwRequestWindowAttention(m_handle);
}


void window::swap_buffers()
{
	glfwSwapBuffers(m_handle);
}


std::string window::title() const
{
	return m_title;
}

void window::set_title(const std::string& title)
{
	m_title = title;
	glfwSetWindowTitle(m_handle, title.c_str());
}


bool window::should_close() const
{
	return glfwWindowShouldClose(m_handle);
}

void window::set_should_close(bool value)
{
	glfwSetWindowShouldClose(m_handle, value);
}


void window::position_callback_raw(GLFWwindow* handle, int xpos, int ypos)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_position_callback)
		wnd.m_position_callback(wnd, xpos, ypos);
}

void window::size_callback_raw(GLFWwindow* handle, int width, int height)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_size_callback)
		wnd.m_size_callback(wnd, width, height);
}

void window::close_callback_raw(GLFWwindow* handle)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_close_callback)
		wnd.m_close_callback(wnd);
}

void window::refresh_callback_raw(GLFWwindow* handle)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_refresh_callback)
		wnd.m_refresh_callback(wnd);
}

void window::focus_callback_raw(GLFWwindow* handle, int focused)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_focus_callback)
		wnd.m_focus_callback(wnd, focused);
}

void window::minimize_callback_raw(GLFWwindow* handle, int minimized)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_minimize_callback)
		wnd.m_minimize_callback(wnd, minimized);
}

void window::maximize_callback_raw(GLFWwindow* handle, int maximized)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_maximize_callback)
		wnd.m_maximize_callback(wnd, maximized);
}

void window::contentscale_callback_raw(GLFWwindow* handle, float xscale, float yscale)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_contentscale_callback)
		wnd.m_contentscale_callback(wnd, xscale, yscale);
}

void window::framebuffersize_callback_raw(GLFWwindow* handle, int width, int height)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	err::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.m_framebuffersize_callback)
		wnd.m_framebuffersize_callback(wnd, width, height);
}


}