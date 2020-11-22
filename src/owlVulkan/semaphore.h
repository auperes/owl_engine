#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"

namespace owl::vulkan
{
    class semaphore
    {
    public:
        semaphore(const std::shared_ptr<logical_device>& logical_device);
        ~semaphore();

        const VkSemaphore& get_vk_semaphore() const { return _vk_semaphore; }

    private:
        VkSemaphore _vk_semaphore;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan