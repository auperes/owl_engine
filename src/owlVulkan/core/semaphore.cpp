#include "semaphore.h"

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    semaphore::semaphore(const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        auto result = vkCreateSemaphore(_logical_device->get_vk_handle(), &semaphore_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create semaphore");
    }

    semaphore::~semaphore() { vkDestroySemaphore(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan