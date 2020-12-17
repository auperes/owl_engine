#include "descriptor_pool.h"

#include <array>

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    descriptor_pool::descriptor_pool(const std::shared_ptr<logical_device>& logical_device, const uint32_t sets_count)
        : _logical_device(logical_device)
    {
        VkDescriptorPoolSize uniform_pool_size{};
        uniform_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniform_pool_size.descriptorCount = sets_count;

        VkDescriptorPoolSize sampler_pool_size{};
        sampler_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_pool_size.descriptorCount = sets_count;

        std::array<VkDescriptorPoolSize, 2> pool_sizes = {uniform_pool_size, sampler_pool_size};

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = sets_count;

        auto result = vkCreateDescriptorPool(_logical_device->get_vk_handle(), &pool_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create descriptor pool.");
    }

    descriptor_pool::~descriptor_pool() { vkDestroyDescriptorPool(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan