#include "pipeline_layout.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    pipeline_layout::pipeline_layout(const std::shared_ptr<logical_device>& logical_device,
                                     const std::shared_ptr<descriptor_set_layout>& descriptor_set_layout)
        : _logical_device(logical_device)
    {
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = &descriptor_set_layout->get_vk_handle();
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;

        auto result = vkCreatePipelineLayout(_logical_device->get_vk_handle(), &pipeline_layout_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create pipeline layout");
    }

    pipeline_layout::~pipeline_layout() { vkDestroyPipelineLayout(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan