#ifndef GORILLA_SMALLAPPLICATION_HPP
#define GORILLA_SMALLAPPLICATION_HPP

#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <imgui/imgui_internal.h>

#include <vulkan_renderer/environment.hpp>
#include <vulkan_renderer/swapchain.hpp>

#include <vulkan_renderer/shader.hpp>

#include <vulkan_renderer/aos_mesh_primitive.hpp>


namespace vkr = gorilla::vulkan_renderer;

namespace gorilla
{

class small_application
{
public:
	small_application(GLFWwindow* wnd, std::shared_ptr<vkr::environment> env, std::shared_ptr<vkr::swapchain> display);

	void init();

	void run();

	void process_inputs();

	void frame(int image_index);

	[[nodiscard]] inline const vk::RenderPass render_pass() const
	{ return *render_pass_; }

private:
	GLFWwindow* wnd_ = nullptr;

	std::shared_ptr<vkr::environment> env_ = nullptr;
	std::shared_ptr<vkr::swapchain> display_ = nullptr;

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
	vk::raii::DescriptorPool imgui_descriptor_pool_ = nullptr;

	vk::raii::Image depth_image_ = nullptr;
	vk::raii::DeviceMemory depth_image_memory_ = nullptr;
	vk::raii::ImageView depth_view_ = nullptr;

	vkr::shader vertex_shader_{ };
	vkr::shader fragment_shader_{ };
	vk::raii::ShaderModule vert_shader_module_ = nullptr;
	vk::raii::ShaderModule frag_shader_module_ = nullptr;
	vk::raii::DescriptorSetLayout descriptor_set_layout_ = nullptr;
	vk::raii::PipelineLayout pipeline_layout_ = nullptr;
	vk::raii::Pipeline pipeline_ = nullptr;

	vkr::aos_mesh_primitive aos_mesh_primitive_{ };

	vk::raii::Buffer uniform_buffer_ = nullptr;
	vk::raii::DeviceMemory uniform_buffer_memory_ = nullptr;

	vk::raii::DescriptorPool descriptor_pool_ = nullptr;
	vk::raii::DescriptorSet descriptor_set_ = nullptr;
};

} // gorilla

#endif //GORILLA_SMALLAPPLICATION_HPP
