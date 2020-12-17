#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "command_pool.h"
#include "device_memory.h"
#include "logical_device.h"
#include "physical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class image : public vulkan_object<VkImage>
    {
    public:
        image(const std::shared_ptr<physical_device>& physical_device,
              const std::shared_ptr<logical_device>& logical_device,
              const uint32_t width,
              const uint32_t height,
              const uint32_t mip_levels,
              VkSampleCountFlagBits samples,
              VkFormat format,
              VkImageTiling tiling,
              VkImageUsageFlags usage,
              VkMemoryPropertyFlags properties);

        image(image&&) = default;
        ~image();

        VkFormat get_format() const { return _format; }

        void transition_layout(const std::shared_ptr<command_pool>& command_pool, VkImageLayout new_layout);
        void copy_buffer(const std::shared_ptr<command_pool>& command_pool, const VkBuffer& source_buffer);
        void generate_mipmaps(const std::shared_ptr<command_pool>& command_pool);

    private:
        std::shared_ptr<logical_device> _logical_device;
        std::unique_ptr<device_memory> _device_memory;
        VkFormat _format;
        VkImageLayout _layout;
        uint32_t _width;
        uint32_t _height;
        uint32_t _mip_levels;

        void process_transition_layout(VkImageLayout new_layout, const VkCommandBuffer& vk_command_buffer);
        void copy_buffer(const VkCommandBuffer& vk_command_buffer, const VkBuffer& source_buffer);
        void process_mipmaps_generation(const VkCommandBuffer& vk_command_buffer);
    };
} // namespace owl::vulkan