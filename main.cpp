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
#include <vulkan_renderer/swapchain.hpp>

#include "smallapplication.hpp"

#include <fstream>
#include <shaderc/shaderc.hpp>

#include <vulkan_renderer/instance.hpp>
#include <vulkan_renderer/surface.hpp>


using namespace gorilla;

void compile_vertex_shader()
{
	std::ifstream file("../shader.vert", std::ios::in | std::ios::ate);

	if (!file.is_open())
	{
		std::cerr << "couldn't open file \"../shader.vert\"" << std::endl;
		return;
	}

	const std::streamoff size = file.tellg();
	std::vector<char> buffer(size);
	file.seekg(0);
	file.read(buffer.data(), size);
	std::string source(buffer.begin(), buffer.end());

	file.close();

	shaderc::CompileOptions compile_options;
	compile_options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
	compile_options.SetTargetSpirv(shaderc_spirv_version_1_3);
	shaderc::Compiler cmp;
	const shaderc::CompilationResult<uint32_t> result = cmp.CompileGlslToSpv(
		source, shaderc_vertex_shader, "shader.vert", compile_options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << "compilation failed:\n" << result.GetErrorMessage() << std::endl;
		return;
	}
	std::vector<uint32_t> spirv(result.begin(), result.end());

	std::ofstream file_out("../shader_vert.spv", std::ios::out | std::ios::binary);
	if (!file_out.is_open())
	{
		std::cerr << "couldn't open file \"../shader_vert.spv\"" << std::endl;
		return;
	}

	file_out.write(reinterpret_cast<char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
	file_out.close();
}

void compile_fragment_shader()
{
	std::ifstream file("../shader.frag", std::ios::in | std::ios::ate);

	if (!file.is_open())
	{
		std::cerr << "couldn't open file \"../shader.frag\"" << std::endl;
		return;
	}

	const std::streamoff size = file.tellg();
	std::vector<char> buffer(size);
	file.seekg(0);
	file.read(buffer.data(), size);
	std::string source(buffer.begin(), buffer.end());

	file.close();

	shaderc::CompileOptions compile_options;
	compile_options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
	compile_options.SetTargetSpirv(shaderc_spirv_version_1_3);
	shaderc::Compiler cmp;
	const shaderc::CompilationResult<uint32_t> result = cmp.CompileGlslToSpv(
		source, shaderc_fragment_shader, "shader.frag", compile_options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << "compilation failed:\n" << result.GetErrorMessage() << std::endl;
		return;
	}
	std::vector<uint32_t> spirv(result.begin(), result.end());

	std::ofstream file_out("../shader_frag.spv", std::ios::out | std::ios::binary);
	if (!file_out.is_open())
	{
		std::cerr << "couldn't open file \"../shader_frag.spv\"" << std::endl;
		return;
	}

	file_out.write(reinterpret_cast<char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
	file_out.close();
}

namespace vkb = gorilla::vulkan_renderer::boilerplate;

int main()
{
//	compile_vertex_shader();
//	compile_fragment_shader();
//	return 0;

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


	vk::raii::SurfaceKHR surface = std::move(glfw::surface(env->instance(), window));
	std::shared_ptr<vkr::swapchain> display = std::make_shared<vkr::swapchain>(env, *surface);

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
