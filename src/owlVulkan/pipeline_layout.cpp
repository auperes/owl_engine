#include "pipeline_layout.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    pipeline_layout::pipeline_layout(const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;

        auto result = vkCreatePipelineLayout(_logical_device->get_vk_device(), &pipeline_layout_info, nullptr, &_vk_pipeline_layout);
        vulkan::helpers::handle_result(result, "Failed to create pipeline layout");
    }

    pipeline_layout::~pipeline_layout() { vkDestroyPipelineLayout(_logical_device->get_vk_device(), _vk_pipeline_layout, nullptr); }
} // namespace owl::vulkan