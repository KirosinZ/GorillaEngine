#include "glfw_cpp/window.hpp"

#include <error_handling/assert.hpp>
#include <utility>


using namespace gorilla;

error_handling::result<void> set_window_hints(const glfw::window_init_info& init_info)
{
	const char* err;

	glfwWindowHint(GLFW_RED_BITS, init_info.framebuffer_hints.red_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_GREEN_BITS, init_info.framebuffer_hints.green_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_BLUE_BITS, init_info.framebuffer_hints.blue_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_ALPHA_BITS, init_info.framebuffer_hints.alpha_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_DEPTH_BITS, init_info.framebuffer_hints.depth_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_STENCIL_BITS, init_info.framebuffer_hints.stencil_bits);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_STEREO, init_info.framebuffer_hints.stereo);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_DOUBLEBUFFER, init_info.framebuffer_hints.doublebuffer);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, init_info.framebuffer_hints.transparent);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_SAMPLES, init_info.framebuffer_hints.samples);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_SRGB_CAPABLE, init_info.framebuffer_hints.srgb_capable);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));


	glfwWindowHint(GLFW_RESIZABLE, init_info.window_hints.resizable);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_DECORATED, init_info.window_hints.decorated);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_FOCUSED, init_info.window_hints.focused);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_AUTO_ICONIFY, init_info.window_hints.minimized);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_FLOATING, init_info.window_hints.top_most);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_MAXIMIZED, init_info.window_hints.maximized);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_VISIBLE, init_info.window_hints.visible);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, init_info.window_hints.scale_to_monitor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CENTER_CURSOR, init_info.window_hints.center_cursor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, init_info.window_hints.focus_on_show);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));


	glfwWindowHint(GLFW_CLIENT_API, static_cast<int32_t>(init_info.context_hints.client));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_CREATION_API, static_cast<int32_t>(init_info.context_hints.source));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, init_info.context_hints.version_major);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, init_info.context_hints.version_minor);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, static_cast<int32_t>(init_info.context_hints.robustness));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, init_info.context_hints.opengl_fwd_compat);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, init_info.context_hints.opengl_debug_context);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, init_info.context_hints.noerror);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int32_t>(init_info.context_hints.opengl_profile));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, static_cast<int32_t>(init_info.context_hints.release));
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	glfwWindowHint(GLFW_REFRESH_RATE, init_info.refresh_rate);
	if (int code = glfwGetError(&err); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err));

	return { };
}

namespace gorilla::glfw
{

window::window(std::string title, GLFWwindow* handle, std::any user_data) noexcept
	: title_(std::move(title)),
	  handle_(handle, &glfwDestroyWindow),
	  user_data_(std::move(user_data))
{
	glfwSetWindowUserPointer(handle_.get(), this);
//	glfwSetWindowPosCallback(handle_.get(), position_callback_raw);
//	glfwSetWindowSizeCallback(handle_.get(), size_callback_raw);
//	glfwSetWindowCloseCallback(handle_.get(), close_callback_raw);
//	glfwSetWindowRefreshCallback(handle_.get(), refresh_callback_raw);
//	glfwSetWindowFocusCallback(handle_.get(), focus_callback_raw);
//	glfwSetWindowIconifyCallback(handle_.get(), minimize_callback_raw);
//	glfwSetWindowMaximizeCallback(handle_.get(), maximize_callback_raw);
//	glfwSetWindowContentScaleCallback(handle_.get(), contentscale_callback_raw);
//	glfwSetFramebufferSizeCallback(handle_.get(), framebuffersize_callback_raw);
}

error_handling::result<window> window::ctor(int32_t width, int32_t height, const std::string& title, const window_init_info& init_info)
{
	window tmp;
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err_msg));

	return window(title, handle);
}

error_handling::result<window> window::ctor(int32_t width, int32_t height, const std::string& title, const monitor& mnt, const window_init_info& init_info)
{
	window tmp;
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), nullptr);
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err_msg));

	return window(title, handle);
}

error_handling::result<window> window::ctor(int32_t width, int32_t height, const std::string& title, const window& share, const window_init_info& init_info)
{
	window tmp;
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), nullptr, share.handle());
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err_msg));

	return window(title, handle);
}

error_handling::result<window> window::ctor(int32_t width, int32_t height, const std::string& title, const monitor& mnt, const window& share, const window_init_info& init_info)
{
	window tmp;
	BOOST_LEAF_CHECK(set_window_hints(init_info));

	GLFWwindow* handle = glfwCreateWindow(width, height, title.c_str(), mnt.handle(), share.handle());
	const char* err_msg;
	if (int code = glfwGetError(&err_msg); code != GLFW_NO_ERROR)
		return error_handling::new_error(glfw::error{ code }, std::string(err_msg));

	return window(title, handle);
}


glm::ivec2 window::position() const
{
	int xpos = 0;
	int ypos = 0;
	glfwGetWindowPos(handle_.get(), &xpos, &ypos);

	return { xpos, ypos };
}

void window::set_position(int xpos, int ypos)
{
	glfwSetWindowPos(handle_.get(), xpos, ypos);
}

glm::ivec2 window::size() const
{
	int width = 0;
	int height = 0;
	glfwGetWindowSize(handle_.get(), &width, &height);

	return { width, height };
}

void window::set_size(int width, int height)
{
	glfwSetWindowSize(handle_.get(), width, height);
}

void window::set_size_limits(int min_width, int min_height, int max_width, int max_height)
{
	glfwSetWindowSizeLimits(handle_.get(), min_width, min_height, max_width, max_height);
}

void window::set_aspect_ratio(int numer, int denom)
{
	glfwSetWindowAspectRatio(handle_.get(), numer, denom);
}

glm::ivec2 window::framebuffer_size() const
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(handle_.get(), &width, &height);

	return { width, height };
}

glm::vec2 window::content_scale() const
{
	float xscale = 0.0f;
	float yscale = 0.0f;
	glfwGetWindowContentScale(handle_.get(), &xscale, &yscale);

	return { xscale, yscale };
}

float window::opacity() const
{
	return glfwGetWindowOpacity(handle_.get());
}

void window::set_opacity(float value)
{
	glfwSetWindowOpacity(handle_.get(), value);
}

void window::minimize()
{
	glfwIconifyWindow(handle_.get());
}

void window::restore()
{
	glfwRestoreWindow(handle_.get());
}

void window::maximize()
{
	glfwMaximizeWindow(handle_.get());
}

void window::show()
{
	glfwShowWindow(handle_.get());
}

void window::hide()
{
	glfwHideWindow(handle_.get());
}

void window::focus()
{
	glfwFocusWindow(handle_.get());
}

void window::request_attention()
{
	glfwRequestWindowAttention(handle_.get());
}


void window::swap_buffers()
{
	glfwSwapBuffers(handle_.get());
}


std::string window::title() const
{
	return title_;
}

void window::set_title(const std::string& title)
{
	title_ = title;
	glfwSetWindowTitle(handle_.get(), title.c_str());
}


bool window::should_close() const
{
	return glfwWindowShouldClose(handle_.get());
}

void window::set_should_close(bool value)
{
	glfwSetWindowShouldClose(handle_.get(), value);
}


void window::position_callback_raw(GLFWwindow* handle, int xpos, int ypos)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.position_callback_)
		wnd.position_callback_(wnd, xpos, ypos);
}

void window::size_callback_raw(GLFWwindow* handle, int width, int height)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.size_callback_)
		wnd.size_callback_(wnd, width, height);
}

void window::close_callback_raw(GLFWwindow* handle)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.close_callback_)
		wnd.close_callback_(wnd);
}

void window::refresh_callback_raw(GLFWwindow* handle)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.refresh_callback_)
		wnd.refresh_callback_(wnd);
}

void window::focus_callback_raw(GLFWwindow* handle, int focused)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.focus_callback_)
		wnd.focus_callback_(wnd, focused);
}

void window::minimize_callback_raw(GLFWwindow* handle, int minimized)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.minimize_callback_)
		wnd.minimize_callback_(wnd, minimized);
}

void window::maximize_callback_raw(GLFWwindow* handle, int maximized)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.maximize_callback_)
		wnd.maximize_callback_(wnd, maximized);
}

void window::contentscale_callback_raw(GLFWwindow* handle, float xscale, float yscale)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.contentscale_callback_)
		wnd.contentscale_callback_(wnd, xscale, yscale);
}

void window::framebuffersize_callback_raw(GLFWwindow* handle, int width, int height)
{
	void* user_ptr = glfwGetWindowUserPointer(handle);
	error_handling::asserts(user_ptr);

	window& wnd = *static_cast<window*>(user_ptr);
	if (wnd.framebuffersize_callback_)
		wnd.framebuffersize_callback_(wnd, width, height);
}


}