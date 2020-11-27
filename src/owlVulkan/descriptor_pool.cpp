#include "descriptor_pool.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    descriptor_pool::descriptor_pool(const std::shared_ptr<logical_device>& logical_device, const uint32_t sets_count)
        : _logical_device(logical_device)
    {
        VkDescriptorPoolSize pool_size{};
        pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size.descriptorCount = sets_count;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = 1;
        pool_info.pPoolSizes = &pool_size;
        pool_info.maxSets = sets_count;

        auto result = vkCreateDescriptorPool(_logical_device->get_vk_handle(), &pool_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create descriptor pool.");
    }

    descriptor_pool::~descriptor_pool() { vkDestroyDescriptorPool(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan