#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"

namespace owl::vulkan
{
    class surface
    {
    public:
        surface(const std::shared_ptr<instance>& instance, VkSurfaceKHR vk_surface);
        ~surface();

        const VkSurfaceKHR& get_vk_surface() const { return _vk_surface; }

    private:
        VkSurfaceKHR _vk_surface;
        std::shared_ptr<instance> _instance;
    };
} // namespace owl::vulkan
