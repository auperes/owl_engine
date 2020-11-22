#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class fence : public vulkan_object<VkFence>
    {
    public:
        fence(const std::shared_ptr<logical_device>& logical_device);
        ~fence();

        void wait_for_fence();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan