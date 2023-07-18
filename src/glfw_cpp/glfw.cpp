#include "glfw.hpp"

namespace glfw
{

err::result<library> initialize()
{
	library library{};

	if (!library.initialized())
		return err::new_error(glfw::error::not_initialized);

	monitor::initialize();

	return library;
}

}