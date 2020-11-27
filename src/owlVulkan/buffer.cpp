#include "buffer.h"

namespace owl::vulkan
{
    buffer::buffer(const std::shared_ptr<physical_device>& physical_device,
                   const std::shared_ptr<logical_device>& logical_device,
                   VkBufferUsageFlags usage,
                   VkSharingMode sharing_mode,
                   VkMemoryPropertyFlags properties,
                   VkDeviceSize size)
        : _logical_device(logical_device)
        , _size(size)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = sharing_mode;

        auto result = vkCreateBuffer(_logical_device->get_vk_handle(), &buffer_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create buffer.");

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(_logical_device->get_vk_handle(), _vk_handle, &memory_requirements);

        VkMemoryAllocateInfo memory_allocate_info{};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = find_memory_type(physical_device, memory_requirements.memoryTypeBits, properties);

        auto allocate_result = vkAllocateMemory(_logical_device->get_vk_handle(), &memory_allocate_info, nullptr, &_vk_device_memory);
        helpers::handle_result(allocate_result, "Failed to allocated buffer memory.");

        vkBindBufferMemory(_logical_device->get_vk_handle(), _vk_handle, _vk_device_memory, 0);
    }

    buffer::~buffer()
    {
        vkDestroyBuffer(_logical_device->get_vk_handle(), _vk_handle, nullptr);
        vkFreeMemory(_logical_device->get_vk_handle(), _vk_device_memory, nullptr);
    }

    uint32_t buffer::find_memory_type(const std::shared_ptr<physical_device>& physical_device,
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

    void buffer::copy_buffer(const VkBuffer& source_buffer, VkDeviceSize size, const std::shared_ptr<command_pool>& command_pool)
    {
        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool = command_pool->get_vk_handle();
        allocate_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(_logical_device->get_vk_handle(), &allocate_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, source_buffer, _vk_handle, 1, &copy_region);

        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(_logical_device->get_vk_graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(_logical_device->get_vk_graphics_queue());

        vkFreeCommandBuffers(_logical_device->get_vk_handle(), command_pool->get_vk_handle(), 1, &command_buffer);
    }
} // namespace owl::vulkan