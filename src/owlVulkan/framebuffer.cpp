#include "framebuffer.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    framebuffer::framebuffer(const std::shared_ptr<vulkan::image_view>& image_view,
                             const std::shared_ptr<render_pass>& render_pass,
                             const std::shared_ptr<swapchain>& swapchain,
                             const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkImageView attachments[] = {image_view->get_vk_image_view()};

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass->get_vk_render_pass();
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swapchain->get_vk_swapchain_extent().width;
        framebuffer_info.height = swapchain->get_vk_swapchain_extent().height;
        framebuffer_info.layers = 1;

        auto result = vkCreateFramebuffer(_logical_device->get_vk_device(), &framebuffer_info, nullptr, &_vk_framebuffer);
        vulkan::helpers::handle_result(result, "Failed to create framebuffers");
    }

    framebuffer::~framebuffer() { vkDestroyFramebuffer(_logical_device->get_vk_device(), _vk_framebuffer, nullptr); }
} // namespace owl::vulkan