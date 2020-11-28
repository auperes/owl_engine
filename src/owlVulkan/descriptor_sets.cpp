#include "descriptor_sets.h"

#include <array>

#include "matrix.h"
#include "vulkan_helpers.h"

namespace owl::vulkan
{
    descriptor_sets::descriptor_sets(const std::shared_ptr<logical_device>& logical_device,
                                     const std::shared_ptr<descriptor_set_layout>& layout,
                                     const std::shared_ptr<descriptor_pool>& descriptor_pool,
                                     const std::vector<std::shared_ptr<vulkan::buffer>>& uniform_buffers,
                                     const std::shared_ptr<image_view>& image_view,
                                     const std::shared_ptr<sampler>& sampler,
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

            VkDescriptorImageInfo image_info{};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = image_view->get_vk_handle();
            image_info.sampler = sampler->get_vk_handle();

            VkWriteDescriptorSet buffer_descriptor_write{};
            buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            buffer_descriptor_write.dstSet = _vk_descriptor_sets[i];
            buffer_descriptor_write.dstBinding = 0;
            buffer_descriptor_write.dstArrayElement = 0;
            buffer_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            buffer_descriptor_write.descriptorCount = 1;
            buffer_descriptor_write.pBufferInfo = &buffer_info;
            buffer_descriptor_write.pImageInfo = nullptr;
            buffer_descriptor_write.pTexelBufferView = nullptr;

            VkWriteDescriptorSet image_descriptor_write{};
            image_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            image_descriptor_write.dstSet = _vk_descriptor_sets[i];
            image_descriptor_write.dstBinding = 1;
            image_descriptor_write.dstArrayElement = 0;
            image_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            image_descriptor_write.descriptorCount = 1;
            image_descriptor_write.pBufferInfo = nullptr;
            image_descriptor_write.pImageInfo = &image_info;
            image_descriptor_write.pTexelBufferView = nullptr;

            std::array<VkWriteDescriptorSet, 2> descriptor_writes = {buffer_descriptor_write, image_descriptor_write};

            vkUpdateDescriptorSets(logical_device->get_vk_handle(),
                                   static_cast<uint32_t>(descriptor_writes.size()),
                                   descriptor_writes.data(),
                                   0,
                                   nullptr);
        }
    }

    descriptor_sets::~descriptor_sets() {}
} // namespace owl::vulkan