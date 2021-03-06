#include "graphics_pipeline.h"

#include "../helpers/vulkan_helpers.h"
#include "shader_module.h"
#include "vertex.h"

namespace owl::vulkan::core
{
    graphics_pipeline::graphics_pipeline(const std::string& vertex_shader_file,
                                         const std::string& fragment_shader_file,
                                         const std::shared_ptr<logical_device>& logical_device,
                                         const std::shared_ptr<swapchain>& swapchain,
                                         const std::shared_ptr<pipeline_layout>& pipeline_layout,
                                         const std::shared_ptr<render_pass>& render_pass,
                                         VkSampleCountFlagBits samples)
        : _logical_device(logical_device)
    {
        // TODO see if shader_module destruction can be done in create_shader_stage_info()
        shader_module vertex_shader_module(vertex_shader_file, _logical_device);
        shader_module fragment_shader_module(fragment_shader_file, _logical_device);

        auto fragment_create_info = create_shader_stage_info(vertex_shader_module.get_vk_handle(), VK_SHADER_STAGE_FRAGMENT_BIT);
        auto vertex_create_info = create_shader_stage_info(fragment_shader_module.get_vk_handle(), VK_SHADER_STAGE_VERTEX_BIT);
        VkPipelineShaderStageCreateInfo shader_stages_infos[] = {vertex_create_info, fragment_create_info};

        VkVertexInputBindingDescription binding_description = get_binding_description();
        auto attribute_descriptions = get_attribute_descriptions();
        auto vertex_input_create_info = create_vertex_input_state_info(binding_description, attribute_descriptions);
        auto input_assembly_create_info = create_input_assembly_state_info();

        VkPipelineViewportStateCreateInfo viewport_state_create_info{};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = VK_NULL_HANDLE; // handled with dynamic states;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = VK_NULL_HANDLE; // handled with dynamic states;

        auto rasterization_state_info = create_rasterization_state_info();
        auto multisample_state_info = create_multisample_state_info(samples);

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        auto color_blend_state_info = create_color_blend_attachment_state_info(color_blend_attachment);

        auto depth_stencil_info = create_depth_stencil_state_info();

        VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamic_state_info{};
        dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_info.dynamicStateCount = 2;
        dynamic_state_info.pDynamicStates = dynamic_states;

        VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
        graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_info.stageCount = 2;
        graphics_pipeline_info.pStages = shader_stages_infos;
        graphics_pipeline_info.pVertexInputState = &vertex_input_create_info;
        graphics_pipeline_info.pInputAssemblyState = &input_assembly_create_info;
        graphics_pipeline_info.pViewportState = &viewport_state_create_info;
        graphics_pipeline_info.pRasterizationState = &rasterization_state_info;
        graphics_pipeline_info.pMultisampleState = &multisample_state_info;
        graphics_pipeline_info.pDepthStencilState = &depth_stencil_info;
        graphics_pipeline_info.pColorBlendState = &color_blend_state_info;
        graphics_pipeline_info.pDynamicState = &dynamic_state_info;
        graphics_pipeline_info.layout = pipeline_layout->get_vk_handle();
        graphics_pipeline_info.renderPass = render_pass->get_vk_handle();
        graphics_pipeline_info.subpass = 0;
        graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        graphics_pipeline_info.basePipelineIndex = -1;

        auto result =
            vkCreateGraphicsPipelines(_logical_device->get_vk_handle(), VK_NULL_HANDLE, 1, &graphics_pipeline_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create graphics pipeline");
    }

    graphics_pipeline::~graphics_pipeline() { vkDestroyPipeline(_logical_device->get_vk_handle(), _vk_handle, nullptr); }

    VkPipelineShaderStageCreateInfo graphics_pipeline::create_shader_stage_info(const VkShaderModule& shader_module,
                                                                                VkShaderStageFlagBits shader_stage)
    {
        VkPipelineShaderStageCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_info.stage = shader_stage;
        create_info.module = shader_module;
        create_info.pName = "main";

        return create_info;
    }

    VkPipelineVertexInputStateCreateInfo graphics_pipeline::create_vertex_input_state_info(
        const VkVertexInputBindingDescription& binding_description,
        const std::array<VkVertexInputAttributeDescription, 3>& attribute_descriptions)
    {
        VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
        vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_create_info.vertexBindingDescriptionCount = 1;
        vertex_input_create_info.pVertexBindingDescriptions = &binding_description;
        vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();

        return vertex_input_create_info;
    }

    VkPipelineInputAssemblyStateCreateInfo graphics_pipeline::create_input_assembly_state_info()
    {
        VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
        input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

        return input_assembly_create_info;
    }

    VkPipelineRasterizationStateCreateInfo graphics_pipeline::create_rasterization_state_info()
    {
        VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
        rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_info.depthClampEnable = VK_FALSE;
        rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_info.lineWidth = 1.0f;
        rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization_state_info.depthBiasEnable = VK_FALSE;
        rasterization_state_info.depthBiasConstantFactor = 0.0f;
        rasterization_state_info.depthBiasClamp = 0.0f;
        rasterization_state_info.depthBiasSlopeFactor = 0.0f;

        return rasterization_state_info;
    }

    VkPipelineMultisampleStateCreateInfo graphics_pipeline::create_multisample_state_info(VkSampleCountFlagBits samples)
    {
        VkPipelineMultisampleStateCreateInfo multisample_state_info{};
        multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state_info.sampleShadingEnable = VK_FALSE;
        multisample_state_info.rasterizationSamples = samples;
        multisample_state_info.minSampleShading = 0.2f;
        multisample_state_info.pSampleMask = nullptr;
        multisample_state_info.alphaToCoverageEnable = VK_FALSE;
        multisample_state_info.alphaToOneEnable = VK_FALSE;

        return multisample_state_info;
    }

    VkPipelineColorBlendStateCreateInfo graphics_pipeline::create_color_blend_attachment_state_info(
        const VkPipelineColorBlendAttachmentState& color_blend_attachment)
    {
        VkPipelineColorBlendStateCreateInfo color_blend_attachment_state_info{};
        color_blend_attachment_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_attachment_state_info.logicOpEnable = VK_FALSE;
        color_blend_attachment_state_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_attachment_state_info.attachmentCount = 1;
        color_blend_attachment_state_info.pAttachments = &color_blend_attachment;
        color_blend_attachment_state_info.blendConstants[0] = 0.0f;
        color_blend_attachment_state_info.blendConstants[1] = 0.0f;
        color_blend_attachment_state_info.blendConstants[2] = 0.0f;
        color_blend_attachment_state_info.blendConstants[3] = 0.0f;

        return color_blend_attachment_state_info;
    }

    VkPipelineDepthStencilStateCreateInfo graphics_pipeline::create_depth_stencil_state_info()
    {
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
        depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_info.depthTestEnable = VK_TRUE;
        depth_stencil_info.depthWriteEnable = VK_TRUE;
        depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_info.minDepthBounds = 0.0f;
        depth_stencil_info.maxDepthBounds = 1.0f;
        depth_stencil_info.stencilTestEnable = VK_FALSE;
        depth_stencil_info.front = {};
        depth_stencil_info.back = {};

        return depth_stencil_info;
    }
} // namespace owl::vulkan