#include "swapchain.h"

#include <algorithm>
#include <functional>

#include "../helpers/vulkan_helpers.h"
#include "queue_families_indices.h"
#include "swapchain_support.h"
#include "vulkan_collections_helpers.h"

namespace owl::vulkan::core
{
    swapchain::swapchain(const std::shared_ptr<physical_device>& physical_device,
                         const std::shared_ptr<logical_device>& logical_device,
                         const std::shared_ptr<surface>& surface,
                         const std::shared_ptr<render_pass>& render_pass,
                         const uint32_t width,
                         const uint32_t height)
        : _physical_device(physical_device)
        , _logical_device(logical_device)
        , _surface(surface)
    {
        VkSwapchainCreateInfoKHR create_info = create_swapchain_info(_physical_device, _surface->get_vk_handle(), width, height);

        auto result = vkCreateSwapchainKHR(_logical_device->get_vk_handle(), &create_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create swap chain");

        _vk_image_format = create_info.imageFormat;
        _vk_extent = create_info.imageExtent;
        _vk_images = get_swapchain_images();

        _color_image =
            create_image(width, height, _vk_image_format, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
        _color_image_view = create_image_view(_color_image->get_vk_handle(), _color_image->get_format(), VK_IMAGE_ASPECT_COLOR_BIT);
        _depth_image = create_image(width, height, _physical_device->get_depth_format(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
        _depth_image_view = create_image_view(_depth_image->get_vk_handle(), _depth_image->get_format(), VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    swapchain::~swapchain() { vkDestroySwapchainKHR(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    VkSurfaceFormatKHR swapchain::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        const auto is_best_surface_format = [](const auto& surface_format) {
            return surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        };

        auto it = std::find_if(available_formats.begin(), available_formats.end(), is_best_surface_format);

        return it != available_formats.end() ? *it : available_formats[0];
    }

    VkPresentModeKHR swapchain::choose_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes)
    {
        const auto is_best_presentation_mode = [](const auto& presentation_mode) {
            return presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR;
        };

        auto it = std::find_if(available_presentation_modes.begin(), available_presentation_modes.end(), is_best_presentation_mode);

        return it != available_presentation_modes.end() ? *it : VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D swapchain::choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D extent = {width, height};
            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return extent;
        }
    }

    VkSwapchainCreateInfoKHR swapchain::create_swapchain_info(const std::shared_ptr<physical_device>& physical_device,
                                                              const VkSurfaceKHR& surface,
                                                              uint32_t width,
                                                              uint32_t height)
    {
        swapchain_support swapchain_support = physical_device->query_swapchain_support();
        VkSurfaceFormatKHR surface_format = choose_surface_format(swapchain_support.formats);
        VkPresentModeKHR presentation_mode = choose_presentation_mode(swapchain_support.presentation_modes);
        VkExtent2D extent = choose_extent(swapchain_support.capabilities, width, height);

        uint32_t images_count = swapchain_support.capabilities.minImageCount + 1;

        if (swapchain_support.capabilities.maxImageCount > 0 && images_count > swapchain_support.capabilities.maxImageCount)
        {
            images_count = swapchain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = surface;
        create_info.minImageCount = images_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        queue_families_indices indices = _physical_device->find_queue_families();
        uint32_t queue_families_indices[] = {indices.graphics_family.value(), indices.presentation_family.value()};

        if (indices.graphics_family != indices.presentation_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_families_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices = nullptr;
        }

        create_info.preTransform = swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = presentation_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        return create_info;
    }

    std::vector<VkImage> swapchain::get_swapchain_images()
    {
        auto enumerateSwapchainImages = vkGetSwapchainImagesKHR;
        auto function =
            std::bind(enumerateSwapchainImages, _logical_device->get_vk_handle(), _vk_handle, std::placeholders::_1, std::placeholders::_2);

        return helpers::getElements<VkImage>(function);
    }

    std::shared_ptr<image> swapchain::create_image(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage)
    {
        return std::make_shared<vulkan::core::image>(_physical_device,
                                                     _logical_device,
                                                     width,
                                                     height,
                                                     1,
                                                     _physical_device->get_max_usable_sample_count(),
                                                     format,
                                                     VK_IMAGE_TILING_OPTIMAL,
                                                     usage,
                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    std::shared_ptr<image_view> swapchain::create_image_view(const VkImage& vk_image, VkFormat format, VkImageAspectFlags aspect_flags)
    {
        return std::make_shared<vulkan::core::image_view>(_logical_device, vk_image, 1, format, aspect_flags);
    }

    void swapchain::create_framebuffers(const std::shared_ptr<render_pass>& render_pass)
    {
        _framebuffers.reserve(_vk_images.size());
        for (const auto& image : _vk_images)
        {
            _framebuffers.push_back(std::make_shared<vulkan::core::framebuffer>(_color_image_view,
                                                                                _depth_image_view,
                                                                                image,
                                                                                _vk_image_format,
                                                                                render_pass,
                                                                                _logical_device,
                                                                                _vk_extent.width,
                                                                                _vk_extent.height));
        }
    }
} // namespace owl::vulkan::core
