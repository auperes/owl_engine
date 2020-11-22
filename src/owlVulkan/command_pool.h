#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "physical_device.h"
#include "surface.h"

namespace owl::vulkan
{
    class command_pool
    {
    public:
        command_pool(const std::shared_ptr<logical_device>& logical_device,
                     const std::shared_ptr<physical_device>& physical_device,
                     const std::shared_ptr<surface>& surface);
        ~command_pool();

        const VkCommandPool& get_vk_command_pool() const { return _vk_command_pool; }

    private:
        VkCommandPool _vk_command_pool;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan