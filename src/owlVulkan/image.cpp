#include "image.h"

#include "command_buffers.h"
#include "vulkan_helpers.h"

namespace owl::vulkan
{
    image::image(const std::shared_ptr<physical_device>& physical_device,
                 const std::shared_ptr<logical_device>& logical_device,
                 const uint32_t width,
                 const uint32_t height,
                 const uint32_t mip_levels,
                 VkFormat format,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties)
        : _logical_device(logical_device)
        , _format(format)
        , _layout(VK_IMAGE_LAYOUT_UNDEFINED)
        , _width(width)
        , _height(height)
        , _mip_levels(mip_levels)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = _width;
        image_info.extent.height = _height;
        image_info.extent.depth = 1;
        image_info.mipLevels = mip_levels;
        image_info.arrayLayers = 1;
        image_info.format = _format;
        image_info.tiling = tiling;
        image_info.initialLayout = _layout;
        image_info.usage = usage;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0;

        auto result = vkCreateImage(_logical_device->get_vk_handle(), &image_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create image.");

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(_logical_device->get_vk_handle(), _vk_handle, &memory_requirements);

        _device_memory = std::make_unique<device_memory>(physical_device, _logical_device, memory_requirements, properties);
        vkBindImageMemory(_logical_device->get_vk_handle(), _vk_handle, _device_memory->get_vk_handle(), 0);
    }

    image::~image() { vkDestroyImage(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    void image::transition_layout(const std::shared_ptr<command_pool>& command_pool, VkImageLayout new_layout)
    {
        command_buffers command_buffers(_logical_device, command_pool, 1);

        command_buffers.process_command_buffers(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                [this, new_layout](const VkCommandBuffer& vk_command_buffer, size_t index) {
                                                    process_transition_layout(new_layout, vk_command_buffer);
                                                });
        _logical_device->submit_to_graphics_queue(command_buffers);
    }

    void image::process_transition_layout(VkImageLayout new_layout, const VkCommandBuffer& vk_command_buffer)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _vk_handle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = _mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition.");
        }

        vkCmdPipelineBarrier(vk_command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        _layout = new_layout;
    }

    void image::copy_buffer(const std::shared_ptr<command_pool>& command_pool, const VkBuffer& source_buffer)
    {
        command_buffers command_buffers(_logical_device, command_pool, 1);

        command_buffers.process_command_buffers(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                [this, &source_buffer](const VkCommandBuffer& vk_command_buffer, size_t index) {
                                                    copy_buffer(vk_command_buffer, source_buffer);
                                                });
        _logical_device->submit_to_graphics_queue(command_buffers);
    }

    void image::copy_buffer(const VkCommandBuffer& vk_command_buffer, const VkBuffer& source_buffer)
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {_width, _height, 1};

        vkCmdCopyBufferToImage(vk_command_buffer, source_buffer, _vk_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }

    void image::generate_mipmaps(const std::shared_ptr<command_pool>& command_pool)
    {
        command_buffers command_buffers(_logical_device, command_pool, 1);
        command_buffers.process_command_buffers(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            [this](const VkCommandBuffer& vk_command_buffer, size_t index) { process_mipmaps_generation(vk_command_buffer); });
        _logical_device->submit_to_graphics_queue(command_buffers);
    }

    void image::process_mipmaps_generation(const VkCommandBuffer& vk_command_buffer)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = _vk_handle;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mip_width = _width;
        int32_t mip_height = _height;

        for (uint32_t i = 1; i < _mip_levels; ++i)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(vk_command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mip_width, mip_height, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(vk_command_buffer,
                           _vk_handle,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           _vk_handle,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(vk_command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            if (mip_width > 1)
                mip_width /= 2;
            if (mip_height > 1)
                mip_height /= 2;
        }

        barrier.subresourceRange.baseMipLevel = _mip_levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(vk_command_buffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);
    }
} // namespace owl::vulkan