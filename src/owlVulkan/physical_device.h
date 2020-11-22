#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"
#include "surface.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class physical_device : public vulkan_object<VkPhysicalDevice>
    {
    public:
        physical_device(const std::shared_ptr<instance>& instance,
                        const std::shared_ptr<surface>& surface,
                        const std::vector<const char*>& required_device_extensions);
        ~physical_device();

    private:
        std::shared_ptr<instance> _instance;
        std::shared_ptr<surface> _surface;

        bool is_device_suitable(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
        bool check_device_extension_support(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
    };
} // namespace owl::vulkan