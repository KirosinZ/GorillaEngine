#ifndef GORILLA_GLFW_HPP
#define GORILLA_GLFW_HPP

#include <error_handling/exceptions.hpp>

#include "library.hpp"
#include "monitor.hpp"
#include "window.hpp"

namespace glfw
{

err::result<library> initialize();

}

#endif //GORILLA_GLFW_HPP
