#ifndef GORILLA_DRAWABLE_HPP
#define GORILLA_DRAWABLE_HPP

#include <vulkan/vulkan_raii.hpp>


namespace gorilla::vulkan_renderer
{

class drawable
{
public:
	virtual void draw(const vk::raii::CommandBuffer& cmd) const = 0;
};

}

#endif //GORILLA_DRAWABLE_HPP
