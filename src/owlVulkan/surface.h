#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class surface : public vulkan_object<VkSurfaceKHR>
    {
    public:
        surface(const std::shared_ptr<instance>& instance, VkSurfaceKHR vk_surface);
        ~surface();

    private:
        std::shared_ptr<instance> _instance;
    };
} // namespace owl::vulkan
