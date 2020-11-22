#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "logical_device.h"

namespace owl::vulkan
{
    class shader_module
    {
    public:
        shader_module(const std::string& filename, const std::shared_ptr<logical_device>& logical_device);
        ~shader_module();

        const VkShaderModule& get_vk_shader_module() const { return _vk_shader_module; }

    private:
        VkShaderModule _vk_shader_module;
        std::shared_ptr<logical_device> _logical_device;
    };
} // namespace owl::vulkan