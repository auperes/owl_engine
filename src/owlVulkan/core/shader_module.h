#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "logical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class shader_module : public vulkan_object<VkShaderModule>
    {
    public:
        shader_module(const std::string& filename, const std::shared_ptr<logical_device>& logical_device);
        ~shader_module();

    private:
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan