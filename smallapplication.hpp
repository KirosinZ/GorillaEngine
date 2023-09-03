#ifndef GORILLA_SMALLAPPLICATION_HPP
#define GORILLA_SMALLAPPLICATION_HPP

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <imgui/imgui_internal.h>

#include <vulkan_renderer/environment.hpp>
#include <vulkan_renderer/display.hpp>


namespace vkr = gorilla::vulkan_renderer;

namespace gorilla
{

class small_application
{
public:
	small_application(GLFWwindow* wnd, std::shared_ptr<vkr::environment> env, std::shared_ptr<vkr::display> display);

	void init();

	void run();

	void process_inputs();

	void frame(int image_index);

	[[nodiscard]] inline const vk::RenderPass render_pass() const
	{ return *render_pass_; }

private:
	GLFWwindow* wnd_ = nullptr;

	std::shared_ptr<vkr::environment> env_ = nullptr;
	std::shared_ptr<vkr::display> display_ = nullptr;

	vk::raii::CommandPool command_pool_ = nullptr;
	std::vector<vk::raii::CommandBuffer> cmds_;

	vk::raii::RenderPass render_pass_ = nullptr;
	std::vector<vk::raii::Framebuffer> framebuffers_{ };

	int frame_index = 0;
	int total_frames = 0;
	std::vector<vk::raii::Fence> frame_fences{ };
	std::vector<vk::raii::Semaphore> image_acquisition_semaphores{ };

	std::vector<vk::raii::Semaphore> image_presentation_semaphores{ };

	std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)> imgui_context_{ nullptr, ImGui::DestroyContext };
};

} // gorilla

#endif //GORILLA_SMALLAPPLICATION_HPP
