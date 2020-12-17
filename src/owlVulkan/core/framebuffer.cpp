#include "framebuffer.h"

#include <array>

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    framebuffer::framebuffer(const std::shared_ptr<image_view>& color_image_view,
                             const std::shared_ptr<image_view>& depth_image_view,
                             const VkImage& vk_image,
                             const VkFormat& vk_image_format,
                             const std::shared_ptr<render_pass>& render_pass,
                             const std::shared_ptr<logical_device>& logical_device,
                             const uint32_t width,
                             const uint32_t height)
        : _logical_device(logical_device)
    {
        _image_view = std::make_shared<image_view>(_logical_device, vk_image, 1, vk_image_format, VK_IMAGE_ASPECT_COLOR_BIT);

        std::array<VkImageView, 3> attachments = {color_image_view->get_vk_handle(),
                                                  depth_image_view->get_vk_handle(),
                                                  _image_view->get_vk_handle()};

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass->get_vk_handle();
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = width;
        framebuffer_info.height = height;
        framebuffer_info.layers = 1;

        auto result = vkCreateFramebuffer(_logical_device->get_vk_handle(), &framebuffer_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create framebuffers");
    }

    framebuffer::~framebuffer() { vkDestroyFramebuffer(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan::core