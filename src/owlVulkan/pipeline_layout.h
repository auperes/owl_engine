#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "logical_device.h"

namespace owl::vulkan
{
    class pipeline_layout
    {
    public:
        pipeline_layout(const std::shared_ptr<logical_device>& logical_device);
        ~pipeline_layout();

        const VkPipelineLayout& get_vk_pipeline_layout() const { return _vk_pipeline_layout; }

    private:
        VkPipelineLayout _vk_pipeline_layout;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan