#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace owl::vulkan
{
    struct swapchain_support
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentation_modes;
    };
}