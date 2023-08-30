#ifndef GORILLA_VULKAN_GLUE_HPP
#define GORILLA_VULKAN_GLUE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "window.hpp"


namespace gorilla::glfw
{

std::vector<const char*> required_instance_extensions();

bool query_present_capabilities(const vk::raii::Instance& t_instance, const vk::raii::PhysicalDevice& t_phys_device, uint32_t queue_family);

vk::raii::SurfaceKHR surface(const vk::raii::Instance& t_instance, const window& t_window);

}

#endif //GORILLA_VULKAN_GLUE_HPP
