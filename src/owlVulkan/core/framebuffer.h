#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "image.h"
#include "image_view.h"
#include "logical_device.h"
#include "render_pass.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class framebuffer : public vulkan_object<VkFramebuffer>
    {
    public:
        framebuffer(const std::shared_ptr<image_view>& color_image_view,
                    const std::shared_ptr<image_view>& depth_image_view,
                    const VkImage& vk_image,
                    const VkFormat& vk_image_format,
                    const std::shared_ptr<render_pass>& render_pass,
                    const std::shared_ptr<logical_device>& logical_device,
                    const uint32_t width,
                    const uint32_t height);
        ~framebuffer();

    private:
        std::shared_ptr<logical_device> _logical_device;
        std::shared_ptr<image_view> _image_view;
    };
} // namespace owl::vulkan::core