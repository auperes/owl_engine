#include "buffer.h"

#include "command_buffers.h"

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

        _device_memory = std::make_unique<device_memory>(physical_device, _logical_device, memory_requirements, properties);
        vkBindBufferMemory(_logical_device->get_vk_handle(), _vk_handle, get_vk_device_memory(), 0);
    }

    buffer::~buffer() { vkDestroyBuffer(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    void buffer::copy_buffer(const VkBuffer& source_buffer, VkDeviceSize size, const std::shared_ptr<command_pool>& command_pool)
    {
        command_buffers command_buffers(_logical_device, command_pool, 1);
        command_buffers.process_command_buffers(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                [this, size, &source_buffer](const VkCommandBuffer& vk_command_buffer, size_t index) {
                                                    VkBufferCopy copy_region{};
                                                    copy_region.size = size;
                                                    vkCmdCopyBuffer(vk_command_buffer, source_buffer, _vk_handle, 1, &copy_region);
                                                });

        _logical_device->submit_to_graphics_queue(command_buffers);
    }

    void copy_memory(const void* values,
                     const std::shared_ptr<logical_device> logical_device,
                     const buffer& buffer,
                     const VkDeviceSize buffer_size)
    {
        void* data;
        vkMapMemory(logical_device->get_vk_handle(), buffer.get_vk_device_memory(), 0, buffer_size, 0, &data);
        memcpy(data, values, (size_t)buffer_size);
        vkUnmapMemory(logical_device->get_vk_handle(), buffer.get_vk_device_memory());
    }

    std::shared_ptr<buffer> create_staging_buffer(const void* values,
                                                  const std::shared_ptr<physical_device> physical_device,
                                                  const std::shared_ptr<logical_device> logical_device,
                                                  const VkDeviceSize buffer_size)
    {
        auto staging_buffer = std::make_shared<buffer>(physical_device,
                                                       logical_device,
                                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                       VK_SHARING_MODE_EXCLUSIVE,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                       buffer_size);

        copy_memory(values, logical_device, *staging_buffer, buffer_size);

        return staging_buffer;
    }
} // namespace owl::vulkan