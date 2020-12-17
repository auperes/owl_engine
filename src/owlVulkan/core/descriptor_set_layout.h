#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class descriptor_set_layout : public vulkan_object<VkDescriptorSetLayout>
    {
    public:
        descriptor_set_layout(const std::shared_ptr<logical_device>& logical_device);
        ~descriptor_set_layout();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan