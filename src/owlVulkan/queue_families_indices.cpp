#include "queue_families_indices.h"

#include <vector>

namespace owl::vulkan
{
    bool queue_families_indices::is_complete()
    {
        return graphics_family.has_value() && presentation_family.has_value();
    }

    queue_families_indices find_queue_families(const VkPhysicalDevice& device,
                                               const VkSurfaceKHR& surface)
    {
        queue_families_indices indices;

        uint32_t queue_families_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_families.data());

        int i = 0;
        for (const auto& queue_family : queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

            if (present_support)
            {
                indices.presentation_family = i;
            }

            if (indices.is_complete())
            {
                break;
            }

            i++;
        }

        return indices;
    }
}
