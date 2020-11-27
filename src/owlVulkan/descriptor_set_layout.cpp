#include "descriptor_set_layout.h"

#include "vulkan_helpers.h"

namespace owl::vulkan
{
    descriptor_set_layout::descriptor_set_layout(const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkDescriptorSetLayoutBinding layout_binding;
        layout_binding.binding = 0;
        layout_binding.descriptorCount = 1;
        layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding.pImmutableSamplers = nullptr;
        layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings = &layout_binding;

        auto result = vkCreateDescriptorSetLayout(_logical_device->get_vk_handle(), &layout_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create descriptor set layout");
    }

    descriptor_set_layout::~descriptor_set_layout() { vkDestroyDescriptorSetLayout(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan