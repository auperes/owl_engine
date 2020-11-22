#include "swapchain_support.h"

#include "vulkan_collections_helpers.h"

namespace owl::vulkan
{
    swapchain_support query_swapchain_support(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
    {
        swapchain_support details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        details.formats = vulkan::helpers::get_physical_device_surface_formats(device, surface);
        details.presentation_modes = vulkan::helpers::get_physical_device_surface_present_modes(device, surface);

        return details;
    }
}