#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class render_pass : public vulkan_object<VkRenderPass>
    {
    public:
        render_pass(const std::shared_ptr<logical_device>& logical_device,
                    const VkFormat color_format,
                    const VkFormat depth_format,
                    VkSampleCountFlagBits samples);
        ~render_pass();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan