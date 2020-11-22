#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace owl::vulkan
{
    class render_pass
    {
    public:
        render_pass(const std::shared_ptr<logical_device>& logical_device, const VkFormat& format);
        ~render_pass();

        const VkRenderPass& get_vk_render_pass() const { return _vk_render_pass; }

    private:
        VkRenderPass _vk_render_pass;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan