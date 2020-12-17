#include "physical_device.h"

#include <functional>
#include <set>

#include "queue_families_indices.h"
#include "swapchain_support.h"
#include "vulkan_collections_helpers.h"

namespace owl::vulkan::core
{
    physical_device::physical_device(const std::shared_ptr<instance>& instance,
                                     const std::shared_ptr<surface>& surface,
                                     const std::vector<const char*>& required_device_extensions)
        : _instance(instance)
        , _surface(surface)
    {
        std::vector<VkPhysicalDevice> devices = _instance->get_physical_devices();

        if (devices.size() == 0)
            throw std::runtime_error("No GPUs with Vulkan support found.");

        for (const auto& device : devices)
        {
            if (is_device_suitable(device, required_device_extensions))
            {
                _vk_handle = device;
                break;
            }
        }

        if (_vk_handle == VK_NULL_HANDLE)
            throw std::runtime_error("No suitable device found.");
    }

    physical_device::~physical_device() {}

    bool physical_device::is_device_suitable(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions)
    {
        vulkan::queue_families_indices indices = find_queue_families(device);
        bool are_required_extensions_supported = check_device_extension_support(device, required_device_extensions);

        bool is_swapchain_suitable = false;
        if (are_required_extensions_supported)
        {
            swapchain_support swapchain_support = query_swapchain_support(device);
            is_swapchain_suitable = !swapchain_support.formats.empty() && !swapchain_support.presentation_modes.empty();
        }

        VkPhysicalDeviceFeatures supported_features{};
        vkGetPhysicalDeviceFeatures(device, &supported_features);

        return indices.is_complete() && are_required_extensions_supported && is_swapchain_suitable && supported_features.samplerAnisotropy;
    }

    bool physical_device::check_device_extension_support(const VkPhysicalDevice& device,
                                                         const std::vector<const char*>& required_device_extensions)
    {
        std::vector<VkExtensionProperties> available_extensions = get_extension_properties(device);
        std::set<std::string> required_extensions(required_device_extensions.begin(), required_device_extensions.end());

        for (const auto& extension : available_extensions)
            required_extensions.erase(extension.extensionName);

        return required_extensions.empty();
    }

    bool physical_device::supports_linear_filtering(VkFormat format)
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(_vk_handle, format, &format_properties);

        return format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    }

    VkFormat physical_device::get_supported_format(const std::vector<VkFormat>& candidates,
                                                   VkImageTiling tiling,
                                                   VkFormatFeatureFlags features)
    {
        for (auto format : candidates)
        {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(_vk_handle, format, &properties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("Failed to find supported format");
    }

    VkFormat physical_device::get_depth_format()
    {
        return get_supported_format({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                    VK_IMAGE_TILING_OPTIMAL,
                                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    swapchain_support physical_device::query_swapchain_support() { return query_swapchain_support(_vk_handle); }

    swapchain_support physical_device::query_swapchain_support(const VkPhysicalDevice& device)
    {
        swapchain_support details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface->get_vk_handle(), &details.capabilities);
        details.formats = get_surface_formats(device);
        details.presentation_modes = get_surface_present_modes(device);

        return details;
    }

    std::vector<VkExtensionProperties> physical_device::get_extension_properties(const VkPhysicalDevice& device, const char* layer_name)
    {
        auto enumerateExtensionProperties = vkEnumerateDeviceExtensionProperties;
        auto function = std::bind(enumerateExtensionProperties, device, layer_name, std::placeholders::_1, std::placeholders::_2);

        return helpers::getElements<VkExtensionProperties>(function);
    }

    std::vector<VkSurfaceFormatKHR> physical_device::get_surface_formats(const VkPhysicalDevice& device)
    {
        auto enumerateSurfaceFormats = vkGetPhysicalDeviceSurfaceFormatsKHR;
        auto function = std::bind(enumerateSurfaceFormats, device, _surface->get_vk_handle(), std::placeholders::_1, std::placeholders::_2);

        return helpers::getElements<VkSurfaceFormatKHR>(function);
    }

    std::vector<VkPresentModeKHR> physical_device::get_surface_present_modes(const VkPhysicalDevice& device)
    {
        auto enumerateSurfacePresentModes = vkGetPhysicalDeviceSurfacePresentModesKHR;
        auto function =
            std::bind(enumerateSurfacePresentModes, device, _surface->get_vk_handle(), std::placeholders::_1, std::placeholders::_2);

        return helpers::getElements<VkPresentModeKHR>(function);
    }

    queue_families_indices physical_device::find_queue_families() { return find_queue_families(_vk_handle); }

    queue_families_indices physical_device::find_queue_families(const VkPhysicalDevice& device)
    {
        queue_families_indices indices;

        uint32_t queue_families_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_families.data());

        int i = 0;
        for (const auto& queue_family : queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface->get_vk_handle(), &present_support);

            if (present_support)
            {
                indices.presentation_family = i;
            }

            if (indices.is_complete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    VkSampleCountFlagBits physical_device::get_max_usable_sample_count()
    {
        VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(_vk_handle, &physical_device_properties);

        VkSampleCountFlags counts =
            physical_device_properties.limits.framebufferColorSampleCounts & physical_device_properties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
            return VK_SAMPLE_COUNT_64_BIT;
        if (counts & VK_SAMPLE_COUNT_32_BIT)
            return VK_SAMPLE_COUNT_32_BIT;
        if (counts & VK_SAMPLE_COUNT_16_BIT)
            return VK_SAMPLE_COUNT_16_BIT;
        if (counts & VK_SAMPLE_COUNT_8_BIT)
            return VK_SAMPLE_COUNT_8_BIT;
        if (counts & VK_SAMPLE_COUNT_4_BIT)
            return VK_SAMPLE_COUNT_4_BIT;
        if (counts & VK_SAMPLE_COUNT_2_BIT)
            return VK_SAMPLE_COUNT_2_BIT;

        return VK_SAMPLE_COUNT_1_BIT;
    }
} // namespace owl::vulkan