#ifndef GORILLA_LIBRARY_HPP
#define GORILLA_LIBRARY_HPP

#include "platform/common.hpp"

#include <string>
#include <vector>

#include <GLFW/glfw3.h>

namespace glfw
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

constexpr u32 version_major = GLFW_VERSION_MAJOR;
constexpr u32 version_minor = GLFW_VERSION_MINOR;
constexpr u32 version_revision = GLFW_VERSION_REVISION;

struct library
{
	// Init hints for GLFW are apparently just a bunch of bullshit
	// so I am not going to implement those. At least not yet.
	library();

	library(library&&) noexcept;
	library& operator=(library&&) noexcept;

	library(const library&) = delete;
	library& operator=(const library&) = delete;

	~library() noexcept;

	[[nodiscard]] inline bool initialized() const { return m_initialized; }

private:
	bool m_initialized = false;
	bool m_valid = false;
};

void poll_events();
void wait_events();
void wait_events(double timeout);
void post_empty_event();

}

#endif //GORILLA_LIBRARY_HPP
