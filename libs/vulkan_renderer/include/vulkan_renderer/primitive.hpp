#ifndef GORILLA_PRIMITIVE_HPP
#define GORILLA_PRIMITIVE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <vulkan_renderer/drawable.hpp>


namespace gorilla::vulkan_renderer
{

class primitive : public drawable
{
public:
	virtual std::vector<vk::VertexInputBindingDescription> binding_descriptions() const = 0;

	virtual std::vector<vk::VertexInputAttributeDescription> attribute_descriptions() const = 0;

	virtual std::pair<vk::PrimitiveTopology, bool> topology() const = 0;

	void draw(const vk::raii::CommandBuffer& cmd) const override = 0;
};

}

#endif //GORILLA_PRIMITIVE_HPP
