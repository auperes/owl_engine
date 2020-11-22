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
        image_view(const std::shared_ptr<logical_device>& logical_device, const std::shared_ptr<swapchain>& swapchain, unsigned index);
        ~image_view();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan