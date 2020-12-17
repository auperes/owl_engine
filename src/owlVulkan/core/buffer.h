#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "../helpers/vulkan_helpers.h"
#include "command_pool.h"
#include "device_memory.h"
#include "logical_device.h"
#include "physical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
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
        const VkDeviceMemory& get_vk_device_memory() const { return _device_memory->get_vk_handle(); }

        void copy_buffer(const VkBuffer& source_buffer, VkDeviceSize size, const std::shared_ptr<command_pool>& command_pool);

    private:
        std::shared_ptr<logical_device> _logical_device;
        std::unique_ptr<device_memory> _device_memory;
        size_t _size;
    };

    void copy_memory(const void* values,
                     const std::shared_ptr<logical_device> logical_device,
                     const buffer& buffer,
                     const VkDeviceSize buffer_size);

    std::shared_ptr<buffer> create_staging_buffer(const void* values,
                                                  const std::shared_ptr<physical_device> physical_device,
                                                  const std::shared_ptr<logical_device> logical_device,
                                                  const VkDeviceSize buffer_size);

    template <typename TValue>
    std::shared_ptr<buffer> create_buffer(const std::vector<TValue>& values,
                                          const std::shared_ptr<physical_device> physical_device,
                                          const std::shared_ptr<logical_device> logical_device,
                                          const std::shared_ptr<command_pool>& command_pool,
                                          VkBufferUsageFlags usage)
    {
        VkDeviceSize buffer_size = sizeof(values[0]) * values.size();
        auto staging_buffer = create_staging_buffer(values.data(), physical_device, logical_device, buffer_size);

        auto buffer = std::make_shared<vulkan::core::buffer>(physical_device,
                                                             logical_device,
                                                             usage,
                                                             VK_SHARING_MODE_EXCLUSIVE,
                                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                             buffer_size);

        buffer->copy_buffer(staging_buffer->get_vk_handle(), buffer_size, command_pool);

        return buffer;
    }
} // namespace owl::vulkan::core