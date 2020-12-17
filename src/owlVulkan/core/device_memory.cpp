#include "device_memory.h"

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    device_memory::device_memory(const std::shared_ptr<physical_device>& physical_device,
                                 const std::shared_ptr<logical_device>& logical_device,
                                 const VkMemoryRequirements& memory_requirements,
                                 VkMemoryPropertyFlags properties)
        : _logical_device(logical_device)
    {
        VkMemoryAllocateInfo memory_allocate_info{};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = find_memory_type(physical_device, memory_requirements.memoryTypeBits, properties);

        auto allocate_result = vkAllocateMemory(_logical_device->get_vk_handle(), &memory_allocate_info, nullptr, &_vk_handle);
        helpers::handle_result(allocate_result, "Failed to allocated buffer memory.");
    }

    device_memory::~device_memory() { vkFreeMemory(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    uint32_t device_memory::find_memory_type(const std::shared_ptr<physical_device>& physical_device,
                                             uint32_t type_filter,
                                             VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device->get_vk_handle(), &memory_properties);

        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
        {
            if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type.");
    }
} // namespace owl::vulkan