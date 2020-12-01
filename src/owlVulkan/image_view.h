#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "logical_device.h"
#include "swapchain.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class image_view : public vulkan_object<VkImageView>
    {
    public:
        image_view(const std::shared_ptr<logical_device>& logical_device,
                   const VkImage& image,
                   const uint32_t mip_levels,
                   VkFormat format,
                   VkImageAspectFlags aspect_flags);
        ~image_view();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan