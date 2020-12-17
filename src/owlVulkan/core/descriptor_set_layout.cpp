#include "descriptor_set_layout.h"

#include <array>

#include "vulkan_helpers.h"

namespace owl::vulkan::core
{
    descriptor_set_layout::descriptor_set_layout(const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkDescriptorSetLayoutBinding uniform_layout_binding{};
        uniform_layout_binding.binding = 0;
        uniform_layout_binding.descriptorCount = 1;
        uniform_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniform_layout_binding.pImmutableSamplers = nullptr;
        uniform_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding sampler_layout_binding{};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uniform_layout_binding, sampler_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings = bindings.data();

        auto result = vkCreateDescriptorSetLayout(_logical_device->get_vk_handle(), &layout_info, nullptr, &_vk_handle);
        helpers::handle_result(result, "Failed to create descriptor set layout");
    }

    descriptor_set_layout::~descriptor_set_layout() { vkDestroyDescriptorSetLayout(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan