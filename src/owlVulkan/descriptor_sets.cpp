#include "descriptor_sets.h"

#include "matrix.h"
#include "vulkan_helpers.h"

namespace owl::vulkan
{
    descriptor_sets::descriptor_sets(const std::shared_ptr<logical_device>& logical_device,
                                     const std::shared_ptr<descriptor_set_layout>& layout,
                                     const std::shared_ptr<descriptor_pool>& descriptor_pool,
                                     const std::vector<std::shared_ptr<vulkan::buffer>>& uniform_buffers,
                                     const uint32_t sets_count)
    {
        std::vector<VkDescriptorSetLayout> layouts(sets_count, layout->get_vk_handle());
        VkDescriptorSetAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocate_info.descriptorPool = descriptor_pool->get_vk_handle();
        allocate_info.descriptorSetCount = sets_count;
        allocate_info.pSetLayouts = layouts.data();

        _vk_descriptor_sets.resize(sets_count);
        auto result = vkAllocateDescriptorSets(logical_device->get_vk_handle(), &allocate_info, _vk_descriptor_sets.data());
        helpers::handle_result(result, "Failed to allocate descriptor sets.");

        for (size_t i = 0; i < sets_count; ++i)
        {
            VkDescriptorBufferInfo buffer_info{};
            buffer_info.buffer = uniform_buffers[i]->get_vk_handle();
            buffer_info.offset = 0;
            buffer_info.range = sizeof(model_view_projection);

            VkWriteDescriptorSet descriptor_write{};
            descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstSet = _vk_descriptor_sets[i];
            descriptor_write.dstBinding = 0;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_write.descriptorCount = 1;
            descriptor_write.pBufferInfo = &buffer_info;
            descriptor_write.pImageInfo = nullptr;
            descriptor_write.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(logical_device->get_vk_handle(), 1, &descriptor_write, 0, nullptr);
        }
    }

    descriptor_sets::~descriptor_sets() {}
} // namespace owl::vulkan