#include "command_buffers.h"

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    command_buffers::command_buffers(const std::shared_ptr<logical_device>& logical_device,
                                     const std::shared_ptr<command_pool>& command_pool,
                                     const std::vector<std::shared_ptr<vulkan::framebuffer>>& swapchain_framebuffers,
                                     const std::shared_ptr<graphics_pipeline>& graphics_pipeline,
                                     const std::shared_ptr<render_pass>& render_pass,
                                     const std::shared_ptr<swapchain>& swapchain)
        : _logical_device(logical_device)
        , _command_pool(command_pool)
    {
        _vk_command_buffers.resize((swapchain_framebuffers.size()));

        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.commandPool = _command_pool->get_vk_command_pool();
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandBufferCount = static_cast<uint32_t>(_vk_command_buffers.size());

        auto result = vkAllocateCommandBuffers(_logical_device->get_vk_device(), &allocate_info, _vk_command_buffers.data());
        vulkan::helpers::handle_result(result, "Failed to allocate command buffers");

        for (size_t i = 0; i < _vk_command_buffers.size(); ++i)
        {
            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = 0;
            begin_info.pInheritanceInfo = nullptr;

            auto begin_result = vkBeginCommandBuffer(_vk_command_buffers[i], &begin_info);
            vulkan::helpers::handle_result(begin_result, "Failed to begin recording command buffer" + std::to_string(i));

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = render_pass->get_vk_render_pass();
            render_pass_info.framebuffer = swapchain_framebuffers[i]->get_vk_framebuffer();
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = swapchain->get_vk_swapchain_extent();

            VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
            render_pass_info.clearValueCount = 1;
            render_pass_info.pClearValues = &clear_color;

            vkCmdBeginRenderPass(_vk_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(_vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline->get_vk_pipeline());
            vkCmdDraw(_vk_command_buffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(_vk_command_buffers[i]);

            auto end_result = vkEndCommandBuffer(_vk_command_buffers[i]);
            vulkan::helpers::handle_result(end_result, "Failed to end recording command buffer" + std::to_string(i));
        }
    }

    command_buffers::~command_buffers()
    {
        vkFreeCommandBuffers(_logical_device->get_vk_device(),
                             _command_pool->get_vk_command_pool(),
                             static_cast<uint32_t>(_vk_command_buffers.size()),
                             _vk_command_buffers.data());
    }
} // namespace owl::vulkan