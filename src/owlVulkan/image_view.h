#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "logical_device.h"
#include "swapchain.h"

namespace owl::vulkan
{
    class image_view
    {
    public:
        image_view(const std::shared_ptr<logical_device>& logical_device, const std::shared_ptr<swapchain>& swapchain, unsigned index);
        ~image_view();

        const VkImageView& get_vk_image_view() const { return _vk_image_view; }

    private:
        VkImageView _vk_image_view;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan