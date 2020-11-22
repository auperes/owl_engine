#include "logical_device.h"

#include <set>
#include <vector>

#include "queue_families_indices.h"
#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    logical_device::logical_device(const std::shared_ptr<physical_device>& physical_device,
                                   const std::shared_ptr<surface>& surface,
                                   const std::vector<const char*>& device_extensions,
                                   const std::vector<const char*>& validation_layers,
                                   bool enable_validation_layers)
    {
        vulkan::queue_families_indices indices =
            vulkan::find_queue_families(physical_device->get_vk_physical_device(), surface->get_vk_surface());

        std::set<uint32_t> unique_queue_families{indices.graphics_family.value(), indices.presentation_family.value()};
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        queue_create_infos.reserve(unique_queue_families.size());

        float queue_priority = 1.0f;

        for (uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features{};
        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        if (enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
        }
        else
            create_info.enabledLayerCount = 0;

        auto result = vkCreateDevice(physical_device->get_vk_physical_device(), &create_info, nullptr, &_vk_device);
        vulkan::helpers::handle_result(result, "Failed to create logical device");

        vkGetDeviceQueue(_vk_device, indices.graphics_family.value(), 0, &_vk_graphics_queue);
        vkGetDeviceQueue(_vk_device, indices.presentation_family.value(), 0, &_vk_presentation_queue);
    }

    logical_device::~logical_device() { vkDestroyDevice(_vk_device, nullptr); }

    void logical_device::wait_idle() { vkDeviceWaitIdle(_vk_device); }

} // namespace owl::vulkan