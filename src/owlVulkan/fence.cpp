#include "fence.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    fence::fence(const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        auto fence_result = vkCreateFence(_logical_device->get_vk_handle(), &fence_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(fence_result, "Failed to create fence");
    }

    fence::~fence() { vkDestroyFence(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    void fence::wait_for_fence() { vkWaitForFences(_logical_device->get_vk_handle(), 1, &_vk_handle, VK_TRUE, UINT64_MAX); }
} // namespace owl::vulkan