#include "glfw_cpp/library.hpp"

#include <error_handling/error_handling.hpp>


namespace gorilla::glfw
{

int library::init_count_ = 0;

library::library() noexcept
{
	if (init_count_ == 0)
	{
		const bool initialized = glfwInit();
		error_handling::asserts(initialized, "GLFW couldn't be initialized");
	}

	init_count_++;
}

library::library(const library& copy) noexcept
{
	init_count_++;
}

library& library::operator=(const library& copy) noexcept
{
	return *this;
}

library::~library() noexcept
{
	init_count_--;

	if (init_count_ == 0)
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