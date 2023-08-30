#ifndef GORILLA_WINDOWHINTS_HPP
#define GORILLA_WINDOWHINTS_HPP


namespace gorilla::glfw
{

constexpr int32_t dont_care = -1;

struct window_hints
{
	bool resizable = true;
	bool visible = true;
	bool decorated = true;
	bool focused = true;
	bool minimized = true;
	bool top_most = false;
	bool maximized = false;

	bool scale_to_monitor = false;
	bool center_cursor = true;
	bool focus_on_show = true;
};

struct framebuffer_hints
{
	int32_t red_bits = 8;
	int32_t green_bits = 8;
	int32_t blue_bits = 8;
	int32_t alpha_bits = 8;

	int32_t depth_bits = 24;
	int32_t stencil_bits = 8;

	int32_t samples = 0;

	bool stereo = false;
	bool srgb_capable = false;
	bool doublebuffer = true;
	bool transparent = false;
};

enum class context_client_api
{
	none = GLFW_NO_API,
	opengl = GLFW_OPENGL_API,
	opengl_es = GLFW_OPENGL_ES_API,
};

enum class context_creation_api
{
	native = GLFW_NATIVE_CONTEXT_API,
	egl = GLFW_EGL_CONTEXT_API,
};

enum class context_robustness
{
	no_robustness = GLFW_NO_ROBUSTNESS,
	no_reset_notification = GLFW_NO_RESET_NOTIFICATION,
	lose_context_on_reset = GLFW_LOSE_CONTEXT_ON_RESET,
};

enum class context_profile
{
	any = GLFW_OPENGL_ANY_PROFILE,
	compat = GLFW_OPENGL_COMPAT_PROFILE,
	core = GLFW_OPENGL_CORE_PROFILE,
};

enum class context_release_behaviour
{
	none = GLFW_RELEASE_BEHAVIOR_NONE,
	any = GLFW_ANY_RELEASE_BEHAVIOR,
	flush = GLFW_RELEASE_BEHAVIOR_FLUSH,
};

struct context_hints
{
	context_client_api client = context_client_api::none;
	context_creation_api source = context_creation_api::native;
	int32_t version_major = 1;
	int32_t version_minor = 0;
	context_robustness robustness = context_robustness::no_robustness;

	bool opengl_fwd_compat = false;
	bool opengl_debug_context = false;
	bool noerror = false;

	context_profile opengl_profile = context_profile::any;
	context_release_behaviour release = context_release_behaviour::any;
};

struct window_init_info
{
	context_hints context_hints{ };
	window_hints window_hints{ };
	framebuffer_hints framebuffer_hints{ };

	int32_t refresh_rate = dont_care;
};

}

#endif //GORILLA_WINDOWHINTS_HPP
