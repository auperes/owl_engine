#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "framebuffer.h"
#include "image.h"
#include "image_view.h"
#include "logical_device.h"
#include "physical_device.h"
#include "render_pass.h"
#include "surface.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class swapchain : public vulkan_object<VkSwapchainKHR>
    {
    public:
        swapchain(const std::shared_ptr<physical_device>& physical_device,
                  const std::shared_ptr<logical_device>& logical_device,
                  const std::shared_ptr<surface>& surface,
                  const std::shared_ptr<render_pass>& render_pass,
                  const uint32_t width,
                  const uint32_t height);
        ~swapchain();

        const std::vector<VkImage>& get_vk_images() const { return _vk_images; };
        const VkFormat& get_vk_image_format() const { return _vk_image_format; };
        const VkExtent2D& get_vk_extent() const { return _vk_extent; };
        const std::shared_ptr<image>& get_color_image() const { return _color_image; }
        const std::shared_ptr<image>& get_depth_image() const { return _depth_image; }
        const std::vector<std::shared_ptr<framebuffer>>& get_framebuffers() const { return _framebuffers; }

        void create_framebuffers(const std::shared_ptr<render_pass>& render_pass);

    private:
        std::shared_ptr<physical_device> _physical_device;
        std::shared_ptr<logical_device> _logical_device;
        std::shared_ptr<surface> _surface;

        VkFormat _vk_image_format;
        VkExtent2D _vk_extent;

        std::vector<VkImage> _vk_images;
        std::shared_ptr<image> _color_image;
        std::shared_ptr<image_view> _color_image_view;
        std::shared_ptr<image> _depth_image;
        std::shared_ptr<image_view> _depth_image_view;

        std::vector<std::shared_ptr<framebuffer>> _framebuffers;

        VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
        VkPresentModeKHR choose_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes);
        VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

        VkSwapchainCreateInfoKHR create_swapchain_info(const std::shared_ptr<physical_device>& physical_device,
                                                       const VkSurfaceKHR& surface,
                                                       uint32_t width,
                                                       uint32_t height);
        std::vector<VkImage> get_swapchain_images();

        std::shared_ptr<image> create_image(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);
        std::shared_ptr<image_view> create_image_view(const VkImage& vk_image, VkFormat format, VkImageAspectFlags aspect_flags);
    };
} // namespace owl::vulkan::core
