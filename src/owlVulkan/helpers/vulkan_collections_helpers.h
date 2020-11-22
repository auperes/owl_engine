#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace owl::vulkan::helpers
{
    std::vector<VkExtensionProperties> get_device_extension_properties(VkPhysicalDevice device, const char* layer_name = nullptr);
    std::vector<VkExtensionProperties> get_instance_extension_properties(const char* layer_name = nullptr);
    std::vector<VkLayerProperties> get_instance_layer_properties();
    std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance);
    std::vector<VkSurfaceFormatKHR> get_physical_device_surface_formats(VkPhysicalDevice device, VkSurfaceKHR surface);
    std::vector<VkPresentModeKHR> get_physical_device_surface_present_modes(VkPhysicalDevice device, VkSurfaceKHR surface);
    std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain);

    template<typename TElement, typename TFunction>
    std::vector<TElement> getElements(TFunction function);

/////////////////////////////////////////////////TEMPLATE DEFINITIONS//////////////////////////////////////////////////

    template<typename TElement, typename TFunction>
    std::vector<TElement> getElements(TFunction function)
    {
        //TODO handle function results (VK_SUCCESS and VK_INCOMPLETE)
        uint32_t elements_count;
        auto result1 = function(&elements_count, nullptr);
        std::vector<TElement> elements(elements_count);
        auto result2 = function(&elements_count, elements.data());

        return elements;
    }
}
