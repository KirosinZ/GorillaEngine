#include <iostream>

#include <vulkan/vulkan_raii.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_glfw.h>

#include <thread>


#include <error_handling/exceptions.hpp>
#include <error_handling/assert.hpp>

//#include <glfw_cpp/glfw.hpp>
#include <glfw_cpp/vulkan_glue.hpp>
#include <glfw_cpp/glfw.hpp>

#include <vulkan_renderer/environment.hpp>
#include <vulkan_renderer/display.hpp>

#include "smallapplication.hpp"


using namespace gorilla;

int main()
{
	glfw::window window = *glfw::window::ctor(
		1920, 1080, "Gorillionaire", {
			.window_hints = {
				.resizable = true,
				.maximized = true,
			},
			.framebuffer_hints = {
				.doublebuffer = false
			},
		});

	std::shared_ptr<vkr::environment> env = std::make_shared<vkr::environment>(
		vkr::environment_create_info{
			.app_name = window.title(),
			.desired_instance_layers = { "VK_LAYER_KHRONOS_validation" },
			.desired_instance_extensions = glfw::required_instance_extensions(),
			.desired_device_extensions = { "VK_KHR_swapchain" }
		});

	error_handling::error_id err{ };
	std::shared_ptr<vkr::display> display = std::make_shared<vkr::display>(env, window.handle(), err);

	gorilla::small_application app(
		window.handle(),
		env,
		display);

	app.run();


	ImGui_ImplVulkan_DestroyFontUploadObjects();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	return 0;
}
