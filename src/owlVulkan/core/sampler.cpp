#include "sampler.h"

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    sampler::sampler(const std::shared_ptr<logical_device>& logical_device, uint32_t mip_levels)
        : _logical_device(logical_device)
    {
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = 16.0f;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = static_cast<uint32_t>(mip_levels);

        auto result = vkCreateSampler(_logical_device->get_vk_handle(), &sampler_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create sampler");
    }

    sampler::~sampler() { vkDestroySampler(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan