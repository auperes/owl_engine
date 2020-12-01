#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan_object.h"

namespace owl::vulkan
{
    class instance : public vulkan_object<VkInstance>
    {
    public:
        instance(bool enable_validation_layers,
                 const std::vector<const char*>& validation_layers,
                 const std::vector<const char*>& required_extensions);

        ~instance();

        std::vector<VkPhysicalDevice> get_physical_devices();

    private:
        bool check_validation_layer_support(const std::vector<const char*>& validation_layers);
    };
} // namespace owl::vulkan