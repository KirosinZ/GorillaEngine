#include <vulkan_renderer/context.hpp>


namespace gorilla::vulkan_renderer::boilerplate
{

const uint32_t context::instance_version_ = vk::raii::Context{ }.enumerateInstanceVersion();
const std::vector<vk::LayerProperties> context::instance_layer_properties_ = vk::raii::Context{ }.enumerateInstanceLayerProperties();
const std::vector<vk::ExtensionProperties> context::instance_extension_properties_ = vk::raii::Context{ }.enumerateInstanceExtensionProperties();

}