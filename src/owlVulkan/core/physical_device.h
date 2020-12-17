#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"
#include "queue_families_indices.h"
#include "surface.h"
#include "swapchain_support.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class physical_device : public vulkan_object<VkPhysicalDevice>
    {
    public:
        physical_device(const std::shared_ptr<instance>& instance,
                        const std::shared_ptr<surface>& surface,
                        const std::vector<const char*>& required_device_extensions);
        ~physical_device();

        bool supports_linear_filtering(VkFormat format);
        VkFormat get_depth_format();
        queue_families_indices find_queue_families();
        swapchain_support query_swapchain_support();
        VkSampleCountFlagBits get_max_usable_sample_count() const { return _max_usable_samples; };

    private:
        std::shared_ptr<instance> _instance;
        std::shared_ptr<surface> _surface;
        VkSampleCountFlagBits _max_usable_samples = VK_SAMPLE_COUNT_1_BIT;

        bool is_device_suitable(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
        bool check_device_extension_support(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions);
        queue_families_indices find_queue_families(const VkPhysicalDevice& device);
        VkFormat get_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        std::vector<VkExtensionProperties> get_extension_properties(const VkPhysicalDevice& device, const char* layer_name = nullptr);
        std::vector<VkSurfaceFormatKHR> get_surface_formats(const VkPhysicalDevice& device);
        std::vector<VkPresentModeKHR> get_surface_present_modes(const VkPhysicalDevice& device);
        swapchain_support query_swapchain_support(const VkPhysicalDevice& device);
        VkSampleCountFlagBits compute_max_usable_sample_count();
    };
} // namespace owl::vulkan