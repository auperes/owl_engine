#include "vulkan_collections_helpers.h"

#include <functional>

namespace owl::vulkan::helpers
{

    std::vector<VkExtensionProperties> get_device_extension_properties(VkPhysicalDevice device, const char* layer_name)
    {
        auto enumerateExtensionProperties = vkEnumerateDeviceExtensionProperties;
        auto function = std::bind(enumerateExtensionProperties, device, layer_name, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkExtensionProperties>(function);
    }

    std::vector<VkExtensionProperties> get_instance_extension_properties(const char* layer_name)
    {
        auto enumerateExtensionProperties = vkEnumerateInstanceExtensionProperties;
        auto function = std::bind(enumerateExtensionProperties, layer_name, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkExtensionProperties>(function);
    }

    std::vector<VkLayerProperties> get_instance_layer_properties()
    {
        return getElements<VkLayerProperties>(vkEnumerateInstanceLayerProperties);
    }

    std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance)
    {
        auto enumeratePhysicalDevices = vkEnumeratePhysicalDevices;
        auto function = std::bind(enumeratePhysicalDevices, instance, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkPhysicalDevice>(function);
    }

    std::vector<VkSurfaceFormatKHR> get_physical_device_surface_formats(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        auto enumerateSurfaceFormats = vkGetPhysicalDeviceSurfaceFormatsKHR;
        auto function = std::bind(enumerateSurfaceFormats, device, surface, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkSurfaceFormatKHR>(function);
    }

    std::vector<VkPresentModeKHR> get_physical_device_surface_present_modes(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        auto enumerateSurfacePresentModes = vkGetPhysicalDeviceSurfacePresentModesKHR;
        auto function = std::bind(enumerateSurfacePresentModes, device, surface, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkPresentModeKHR>(function);
    }

    std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
    {
        auto enumerateSwapchainImages = vkGetSwapchainImagesKHR;
        auto function = std::bind(enumerateSwapchainImages, device, swapchain, std::placeholders::_1, std::placeholders::_2);

        return getElements<VkImage>(function);
    }


}
