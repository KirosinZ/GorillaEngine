#include "library.hpp"

#include <utility>

namespace glfw
{

library::library()
	: m_initialized(glfwInit()),
	  m_valid(true)
{}

library::library(library&& move) noexcept
	: m_initialized(move.m_initialized),
	  m_valid(std::exchange(move.m_valid, false))
{}

library& library::operator=(library&& move) noexcept
{
	if (this == &move)
		return *this;

	m_initialized = move.m_initialized;
	m_valid = std::exchange(move.m_valid, false);

	return *this;
}

library::~library() noexcept
{
	if (m_valid)
		glfwTerminate();
}

void poll_events()
{
	glfwPollEvents();
}

void wait_events()
{
	glfwWaitEvents();
}

void wait_events(double timeout)
{
	glfwWaitEventsTimeout(timeout);
}

void post_empty_event()
{
	glfwPostEmptyEvent();
}

}