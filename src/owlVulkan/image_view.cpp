#include "image_view.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    image_view::image_view(const std::shared_ptr<logical_device>& logical_device,
                           const VkImage& image,
                           const uint32_t mip_levels,
                           VkFormat format,
                           VkImageAspectFlags aspect_flags)
        : _logical_device(logical_device)
    {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = aspect_flags;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = mip_levels;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        auto result = vkCreateImageView(_logical_device->get_vk_handle(), &create_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create image view");
    }

    image_view::~image_view() { vkDestroyImageView(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan