#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "physical_device.h"
#include "surface.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class command_pool : public vulkan_object<VkCommandPool>
    {
    public:
        command_pool(const std::shared_ptr<logical_device>& logical_device,
                     const std::shared_ptr<physical_device>& physical_device,
                     const std::shared_ptr<surface>& surface);
        ~command_pool();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan