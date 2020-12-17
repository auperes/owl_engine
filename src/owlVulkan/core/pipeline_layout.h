#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "descriptor_set_layout.h"
#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class pipeline_layout : public vulkan_object<VkPipelineLayout>
    {
    public:
        pipeline_layout(const std::shared_ptr<logical_device>& logical_device,
                        const std::shared_ptr<descriptor_set_layout>& descriptor_set_layout);
        ~pipeline_layout();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan