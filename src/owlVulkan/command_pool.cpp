#include "command_pool.h"

#include "helpers/vulkan_helpers.h"
#include "queue_families_indices.h"

namespace owl::vulkan
{
    command_pool::command_pool(const std::shared_ptr<logical_device>& logical_device,
                               const std::shared_ptr<surface>& surface,
                               uint32_t graphics_queue_family_index)
        : _logical_device(logical_device)
    {
        VkCommandPoolCreateInfo command_pool_info{};
        command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.queueFamilyIndex = graphics_queue_family_index;
        command_pool_info.flags = 0;

        auto result = vkCreateCommandPool(_logical_device->get_vk_handle(), &command_pool_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create command pool");
    }

    command_pool::~command_pool() { vkDestroyCommandPool(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan