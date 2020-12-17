#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "surface.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class command_pool : public vulkan_object<VkCommandPool>
    {
    public:
        command_pool(const std::shared_ptr<logical_device>& logical_device,
                     const std::shared_ptr<surface>& surface,
                     uint32_t graphics_queue_family_index);
        ~command_pool();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan