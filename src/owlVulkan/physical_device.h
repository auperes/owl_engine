#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"
#include "surface.h"

namespace owl::vulkan
{
    class physical_device
    {
    public:
        physical_device(const std::shared_ptr<instance>& instance,
                        const std::shared_ptr<surface>& surface,
                        const std::vector<const char*>& required_device_extensions);
        ~physical_device();

        const VkPhysicalDevice& get_vk_physical_device() const { return _vk_physical_device; }

    private:
        VkPhysicalDevice _vk_physical_device;
        std::shared_ptr<instance> _instance;
        std::shared_ptr<surface> _surface;

        bool is_device_suitable(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
        bool check_device_extension_support(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
    };
} // namespace owl::vulkan