#include "command_buffers.h"

#include <array>

#include "../helpers/vulkan_helpers.h"

namespace owl::vulkan::core
{
    command_buffers::command_buffers(const std::shared_ptr<logical_device>& logical_device,
                                     const std::shared_ptr<command_pool>& command_pool,
                                     size_t command_buffer_count)
        : _logical_device(logical_device)
        , _command_pool(command_pool)
    {
        _vk_command_buffers.resize(command_buffer_count);

        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool = _command_pool->get_vk_handle();
        allocate_info.commandBufferCount = static_cast<uint32_t>(_vk_command_buffers.size());

        auto result = vkAllocateCommandBuffers(_logical_device->get_vk_handle(), &allocate_info, _vk_command_buffers.data());
        vulkan::helpers::handle_result(result, "Failed to allocate command buffers");
    }

    command_buffers::~command_buffers()
    {
        vkFreeCommandBuffers(_logical_device->get_vk_handle(),
                             _command_pool->get_vk_handle(),
                             static_cast<uint32_t>(_vk_command_buffers.size()),
                             _vk_command_buffers.data());
    }

    void command_buffers::process_command_buffers(VkCommandBufferUsageFlags begin_flags,
                                                  const std::function<void(const VkCommandBuffer&, size_t)>& action)
    {
        for (size_t i = 0; i < _vk_command_buffers.size(); ++i)
        {
            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = begin_flags;
            begin_info.pInheritanceInfo = nullptr;

            auto begin_result = vkBeginCommandBuffer(_vk_command_buffers[i], &begin_info);
            vulkan::helpers::handle_result(begin_result, "Failed to begin recording command buffer" + std::to_string(i));

            action(_vk_command_buffers[i], i); // TODO consider use a functor instead of lambda

            auto end_result = vkEndCommandBuffer(_vk_command_buffers[i]);
            vulkan::helpers::handle_result(end_result, "Failed to end recording command buffer" + std::to_string(i));
        }
    }

    void process_engine_command_buffer(const VkCommandBuffer& vk_command_buffer,
                                       size_t index,
                                       const std::shared_ptr<graphics_pipeline>& graphics_pipeline,
                                       const std::shared_ptr<render_pass>& render_pass,
                                       const std::shared_ptr<swapchain>& swapchain,
                                       const std::shared_ptr<buffer>& vertex_buffer,
                                       const std::shared_ptr<buffer>& index_buffer,
                                       const std::shared_ptr<descriptor_sets>& descriptor_sets,
                                       const std::shared_ptr<pipeline_layout>& pipeline_layout,
                                       const uint32_t indices_size)
    {
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass->get_vk_handle();
        render_pass_info.framebuffer = swapchain->get_framebuffers()[index]->get_vk_handle();
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = swapchain->get_vk_extent();

        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(vk_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline->get_vk_handle());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain->get_vk_extent().width;
        viewport.height = (float)swapchain->get_vk_extent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain->get_vk_extent();

        vkCmdSetViewport(vk_command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(vk_command_buffer, 0, 1, &scissor);

        VkBuffer vertex_buffers[] = {vertex_buffer->get_vk_handle()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(vk_command_buffer, 0, 1, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(vk_command_buffer, index_buffer->get_vk_handle(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(vk_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline_layout->get_vk_handle(),
                                0,
                                1,
                                &(descriptor_sets->get_vk_descriptor_sets()[index]),
                                0,
                                nullptr);

        vkCmdDrawIndexed(vk_command_buffer, indices_size, 1, 0, 0, 0);

        vkCmdEndRenderPass(vk_command_buffer);
    }
} // namespace owl::vulkan