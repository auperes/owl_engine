#include "vulkan_collections_helpers.h"

#include <functional>

namespace owl::vulkan::helpers
{
    std::vector<VkExtensionProperties> get_instance_extension_properties(const char* layer_name)
    {
        auto enumerateExtensionProperties = vkEnumerateInstanceExtensionProperties;
        auto function = std::bind(enumerateExtensionProperties,
                                  layer_name,
                                  std::placeholders::_1,
                                  std::placeholders::_2); // TODO use lambda instead of bind

        return getElements<VkExtensionProperties>(function);
    }

    std::vector<VkLayerProperties> get_instance_layer_properties()
    {
        return getElements<VkLayerProperties>(vkEnumerateInstanceLayerProperties);
    }
} // namespace owl::vulkan::helpers
