#ifndef GORILLA_GLFW_HPP
#define GORILLA_GLFW_HPP

#include "platform/common.hpp"

#include <GLFW/glfw3.h>

namespace glfw
{

constexpr u32 version_major = GLFW_VERSION_MAJOR;
constexpr u32 version_minor = GLFW_VERSION_MINOR;
constexpr u32 version_revision = GLFW_VERSION_REVISION;

struct library
{
	// Init hints for GLFW are apparently just a bunch of bullshit
	// so I am not going to implement those. At least not yet.
	inline library()
		: m_initialized(glfwInit())
	{}

	inline ~library()
	{
		glfwTerminate();
	}

	library(library&&) = default;
	library& operator=(library&&) = default;

	library(const library&) = delete;
	library& operator=(const library&) = delete;

	[[nodiscard]] inline bool initialized() const { return m_initialized; }

private:
	bool m_initialized = false;
};

}

#endif //GORILLA_GLFW_HPP
