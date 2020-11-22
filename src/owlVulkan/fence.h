#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace owl::vulkan
{
    class fence
    {
    public:
        fence(const std::shared_ptr<logical_device>& logical_device);
        ~fence();

        const VkFence& get_vk_fence() const { return _vk_fence; }

        void wait_for_fence();

    private:
        VkFence _vk_fence;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan