#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "debug_messenger.h"
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
        std::unique_ptr<vulkan::debug_messenger> _debug_messenger;

        bool check_validation_layer_support(const std::vector<const char*>& validation_layers);
    };
} // namespace owl::vulkan