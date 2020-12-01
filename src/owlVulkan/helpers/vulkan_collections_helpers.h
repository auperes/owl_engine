#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace owl::vulkan::helpers
{
    std::vector<VkExtensionProperties> get_instance_extension_properties(const char* layer_name = nullptr);
    std::vector<VkLayerProperties> get_instance_layer_properties();

    template <typename TElement, typename TFunction>
    std::vector<TElement> getElements(TFunction function);

    /////////////////////////////////////////////////TEMPLATE DEFINITIONS//////////////////////////////////////////////////

    template <typename TElement, typename TFunction>
    std::vector<TElement> getElements(TFunction function)
    {
        // TODO handle function results (VK_SUCCESS and VK_INCOMPLETE)
        uint32_t elements_count;
        auto result1 = function(&elements_count, nullptr);
        std::vector<TElement> elements(elements_count);
        auto result2 = function(&elements_count, elements.data());

        return elements;
    }
} // namespace owl::vulkan::helpers
