#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class sampler : public vulkan_object<VkSampler>
    {
    public:
        sampler(const std::shared_ptr<logical_device>& logical_device, uint32_t mip_levels);
        ~sampler();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan