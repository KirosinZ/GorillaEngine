#ifndef GORILLA_LIBRARY_HPP
#define GORILLA_LIBRARY_HPP

#include <string>
#include <vector>

#include <GLFW/glfw3.h>


namespace gorilla::glfw
{

enum class error
{
	none = GLFW_NO_ERROR,
	not_initialized = GLFW_NOT_INITIALIZED,
	no_current_context = GLFW_NO_CURRENT_CONTEXT,
	invalid_enum = GLFW_INVALID_ENUM,
	invalid_value = GLFW_INVALID_VALUE,
	out_of_memory = GLFW_OUT_OF_MEMORY,
	api_unavaliable = GLFW_API_UNAVAILABLE,
	version_unavailable = GLFW_VERSION_UNAVAILABLE,
	platform_error = GLFW_PLATFORM_ERROR,
	format_unavailable = GLFW_FORMAT_UNAVAILABLE,
	no_window_context = GLFW_NO_WINDOW_CONTEXT,
};

constexpr uint32_t version_major = GLFW_VERSION_MAJOR;
constexpr uint32_t version_minor = GLFW_VERSION_MINOR;
constexpr uint32_t version_revision = GLFW_VERSION_REVISION;

struct library
{
	library() noexcept;

	library(const library&) noexcept;

	library& operator=(const library&) noexcept;

	~library() noexcept;

private:
	static int init_count_;
};

void poll_events();

void wait_events();

void wait_events(double timeout);

void post_empty_event();

}

#endif //GORILLA_LIBRARY_HPP
