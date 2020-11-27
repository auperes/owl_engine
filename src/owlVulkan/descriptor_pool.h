#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class descriptor_pool : public vulkan_object<VkDescriptorPool>
    {
    public:
        descriptor_pool(const std::shared_ptr<logical_device>& logical_device, const uint32_t sets_count);
        ~descriptor_pool();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan