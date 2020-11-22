#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "logical_device.h"
#include "physical_device.h"
#include "surface.h"

namespace owl::vulkan
{
    struct swapchain_support
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentation_modes;
    };

    swapchain_support query_swapchain_support(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    VkSwapchainCreateInfoKHR create_swapchain_info(const VkPhysicalDevice& physical_device,
                                                   const VkSurfaceKHR& surface,
                                                   uint32_t width,
                                                   uint32_t height);

    class swapchain
    {
    public:
        swapchain(const std::shared_ptr<physical_device>& physical_device,
                  const std::shared_ptr<logical_device>& logical_device,
                  const std::shared_ptr<surface>& surface,
                  const uint32_t width,
                  const uint32_t height);
        ~swapchain();

        const VkSwapchainKHR& get_vk_swapchain() const { return _vk_swapchain; }

        const std::vector<VkImage>& get_vk_swapchain_images() const { return _vk_swapchain_images; };
        const VkFormat& get_vk_swapchain_image_format() const { return _vk_swapchain_image_format; };
        const VkExtent2D& get_vk_swapchain_extent() const { return _vk_swapchain_extent; };

    private:
        VkSwapchainKHR _vk_swapchain;
        std::shared_ptr<physical_device> _physical_device;
        std::shared_ptr<logical_device> _logical_device;
        std::shared_ptr<surface> _surface;

        std::vector<VkImage> _vk_swapchain_images;
        VkFormat _vk_swapchain_image_format;
        VkExtent2D _vk_swapchain_extent;
    };
} // namespace owl::vulkan