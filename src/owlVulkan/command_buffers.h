#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <vector>

#include "buffer.h"
#include "command_pool.h"
#include "descriptor_sets.h"
#include "framebuffer.h"
#include "graphics_pipeline.h"
#include "logical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "swapchain.h"
#include "vertex.h"

namespace owl::vulkan
{
    class command_buffers
    {
    public:
        command_buffers(const std::shared_ptr<logical_device>& logical_device,
                        const std::shared_ptr<command_pool>& command_pool,
                        size_t command_buffer_count);
        ~command_buffers();

        const std::vector<VkCommandBuffer>& get_vk_command_buffers() const { return _vk_command_buffers; }

        void process_command_buffers(VkCommandBufferUsageFlags begin_flags,
                                     const std::function<void(const VkCommandBuffer&, size_t)>& action);

    private:
        std::vector<VkCommandBuffer> _vk_command_buffers;
        std::shared_ptr<logical_device> _logical_device;
        std::shared_ptr<command_pool> _command_pool;
    };

    void process_engine_command_buffer(const VkCommandBuffer& vk_command_buffer,
                                       size_t index,
                                       const std::vector<std::shared_ptr<vulkan::framebuffer>>& swapchain_framebuffers,
                                       const std::shared_ptr<graphics_pipeline>& graphics_pipeline,
                                       const std::shared_ptr<render_pass>& render_pass,
                                       const std::shared_ptr<swapchain>& swapchain,
                                       const std::shared_ptr<buffer>& vertex_buffer,
                                       const std::shared_ptr<buffer>& index_buffer,
                                       const std::shared_ptr<descriptor_sets>& descriptor_sets,
                                       const std::shared_ptr<pipeline_layout>& pipeline_layout,
                                       const uint32_t indices_size);

} // namespace owl::vulkan