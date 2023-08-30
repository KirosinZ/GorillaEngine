#include "glfw_cpp/vulkan_glue.hpp"

#include <error_handling/assert.hpp>


namespace gorilla::glfw
{

std::vector<const char*> required_instance_extensions()
{
	error_handling::asserts(glfwVulkanSupported(), "Vulkan is not supported");

	uint32_t size = 0;
	const char** exts = glfwGetRequiredInstanceExtensions(&size);
	if (exts == nullptr)
	{
		const char* err_msg;
		const bool glfw_initialized = glfwGetError(&err_msg) == GLFW_NO_ERROR;
		error_handling::asserts(glfw_initialized, err_msg);
	}

	std::vector<const char*> res(size);
	for (int e_ind = 0; e_ind < size; ++e_ind)
	{
		res[e_ind] = exts[e_ind];
	}

	return res;
}

bool query_present_capabilities(const vk::raii::Instance& t_instance, const vk::raii::PhysicalDevice& t_phys_device, uint32_t queue_family)
{
	error_handling::asserts(glfwVulkanSupported(), "Vulkan is not supported");

	bool res = glfwGetPhysicalDevicePresentationSupport(*t_instance, *t_phys_device, queue_family);
	if (!res)
	{
		const char* err_msg;
		const bool glfw_initialized = glfwGetError(&err_msg) == GLFW_NO_ERROR;
		std::string err_str = err_msg == nullptr ? "" : err_msg;
		error_handling::asserts(glfw_initialized, err_str);
	}
	return res;
}

vk::raii::SurfaceKHR surface(const vk::raii::Instance& t_instance, const window& t_window)
{
	error_handling::asserts(glfwVulkanSupported(), "Vulkan is not supported");

	VkSurfaceKHR res;
	glfwCreateWindowSurface(*t_instance, t_window.handle(), nullptr, &res);

	return { t_instance, res };
}

}