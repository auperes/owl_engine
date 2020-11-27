#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "command_pool.h"
#include "helpers/vulkan_helpers.h"
#include "logical_device.h"
#include "physical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class buffer : public vulkan_object<VkBuffer>
    {
    public:
        buffer(const std::shared_ptr<physical_device>& physical_device,
               const std::shared_ptr<logical_device>& logical_device,
               VkBufferUsageFlags usage,
               VkSharingMode sharing_mode,
               VkMemoryPropertyFlags properties,
               VkDeviceSize size);
        ~buffer();

        size_t get_size() const { return _size; }
        const VkDeviceMemory& get_vk_device_memory() const { return _vk_device_memory; }

        void copy_buffer(const VkBuffer& source_buffer, VkDeviceSize size, const std::shared_ptr<command_pool>& command_pool);

    private:
        std::shared_ptr<logical_device> _logical_device;
        VkDeviceMemory _vk_device_memory;
        size_t _size;

        uint32_t find_memory_type(const std::shared_ptr<physical_device>& physical_device,
                                  uint32_t type_filter,
                                  VkMemoryPropertyFlags properties);
    };

    template <typename TValue>
    std::shared_ptr<buffer> create_buffer(const std::vector<TValue>& values,
                                          const std::shared_ptr<physical_device> physical_device,
                                          const std::shared_ptr<logical_device> logical_device,
                                          const std::shared_ptr<command_pool>& command_pool,
                                          VkBufferUsageFlags usage)
    {
        VkDeviceSize buffer_size = sizeof(values[0]) * values.size();
        vulkan::buffer staging_buffer(physical_device,
                                      logical_device,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      buffer_size);

        void* data;
        vkMapMemory(logical_device->get_vk_handle(), staging_buffer.get_vk_device_memory(), 0, buffer_size, 0, &data);
        memcpy(data, values.data(), (size_t)buffer_size);
        vkUnmapMemory(logical_device->get_vk_handle(), staging_buffer.get_vk_device_memory());

        auto buffer = std::make_shared<vulkan::buffer>(physical_device,
                                                       logical_device,
                                                       usage,
                                                       VK_SHARING_MODE_EXCLUSIVE,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       buffer_size);

        buffer->copy_buffer(staging_buffer.get_vk_handle(), buffer_size, command_pool);

        return buffer;
    }
} // namespace owl::vulkan