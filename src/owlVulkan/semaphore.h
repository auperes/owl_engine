#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class semaphore : public vulkan_object<VkSemaphore>
    {
    public:
        semaphore(const std::shared_ptr<logical_device>& logical_device);
        ~semaphore();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan