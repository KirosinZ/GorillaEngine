#ifndef GORILLA_CONTEXT_HPP
#define GORILLA_CONTEXT_HPP

#include <vulkan/vulkan_raii.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

class context
{
public:
	inline static uint32_t instance_version()
	{ return instance_version_; }

	inline static std::vector<vk::LayerProperties> instance_layer_properties()
	{ return instance_layer_properties_; }

	inline static std::vector<vk::ExtensionProperties> instance_extension_properties()
	{ return instance_extension_properties_; }

private:
	static const uint32_t instance_version_;
	static const std::vector<vk::LayerProperties> instance_layer_properties_;
	static const std::vector<vk::ExtensionProperties> instance_extension_properties_;
};

}

#endif //GORILLA_CONTEXT_HPP
