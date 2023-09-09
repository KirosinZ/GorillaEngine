#ifndef GORILLA_FRAME_HPP
#define GORILLA_FRAME_HPP

#include <vulkan_renderer/swapchain.hpp>


namespace gorilla::vulkan_renderer
{

class frame
{
public:
	frame(std::shared_ptr<environment> env, std::shared_ptr<swapchain> display);

private:
	std::shared_ptr<environment> env_ = nullptr;
	std::shared_ptr<swapchain> display_ = nullptr;

	vk::raii::Image depth_image_ = nullptr;
	vk::raii::DeviceMemory depth_image_memory_ = nullptr;
	vk::raii::ImageView depth_view_ = nullptr;
	vk::Format depth_format_{ };
};

}

#endif //GORILLA_FRAME_HPP
