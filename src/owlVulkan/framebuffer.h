#pragma once

#include <vulkan/vulkan.h>

#include "image_view.h"
#include "logical_device.h"
#include "render_pass.h"
#include "swapchain.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class framebuffer : public vulkan_object<VkFramebuffer>
    {
    public:
        framebuffer(const std::shared_ptr<vulkan::image_view>& color_image_view,
                    const std::shared_ptr<vulkan::image_view>& depth_image_view,
                    const std::shared_ptr<vulkan::image_view>& image_view,
                    const std::shared_ptr<render_pass>& render_pass,
                    const std::shared_ptr<swapchain>& swapchain,
                    const std::shared_ptr<logical_device>& logical_device);
        ~framebuffer();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan