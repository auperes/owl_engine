#pragma once

#include <memory>
#include <string>

#include "logical_device.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "swapchain.h"

namespace owl::vulkan
{
    class graphics_pipeline : public pipeline
    {
    public:
        graphics_pipeline(const std::string& vertex_shader_file,
                          const std::string& fragment_shader_file,
                          const std::shared_ptr<logical_device>& logical_device,
                          const std::shared_ptr<swapchain>& swapchain,
                          const std::shared_ptr<pipeline_layout>& pipeline_layout,
                          const std::shared_ptr<render_pass>& render_pass);
        ~graphics_pipeline();

    private:
        std::shared_ptr<logical_device> _logical_device;

        VkPipelineShaderStageCreateInfo create_shader_stage_info(const VkShaderModule& shader_module, VkShaderStageFlagBits shader_stage);
        VkPipelineVertexInputStateCreateInfo create_vertex_input_state_info(
            const VkVertexInputBindingDescription& binding_description,
            const std::array<VkVertexInputAttributeDescription, 3>& attribute_descriptions);
        VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state_info();
        VkPipelineRasterizationStateCreateInfo create_rasterization_state_info();
        VkPipelineMultisampleStateCreateInfo create_multisample_state_info();
        VkPipelineColorBlendStateCreateInfo create_color_blend_attachment_state_info(
            const VkPipelineColorBlendAttachmentState& color_blend_attachment);
    };
} // namespace owl::vulkan