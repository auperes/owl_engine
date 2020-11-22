#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace owl::vulkan
{
    class instance
    {
    public:
        instance(bool enable_validation_layers,
                 const std::vector<const char*>& validation_layers,
                 const std::vector<const char*>& required_extensions);

        ~instance();

        const VkInstance& get_vk_instance() const { return _vk_instance; }

    private:
        VkInstance _vk_instance;

        bool check_validation_layer_support(const std::vector<const char*>& validation_layers);
    };
} // namespace owl::vulkan