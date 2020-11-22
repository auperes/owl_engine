#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "command_pool.h"
#include "framebuffer.h"
#include "graphics_pipeline.h"
#include "logical_device.h"
#include "render_pass.h"
#include "swapchain.h"

namespace owl::vulkan
{
    class command_buffers
    {
    public:
        command_buffers(const std::shared_ptr<logical_device>& logical_device,
                        const std::shared_ptr<command_pool>& command_pool,
                        const std::vector<std::shared_ptr<vulkan::framebuffer>>& swapchain_framebuffers,
                        const std::shared_ptr<graphics_pipeline>& graphics_pipeline,
                        const std::shared_ptr<render_pass>& render_pass,
                        const std::shared_ptr<swapchain>& swapchain);
        ~command_buffers();

        const std::vector<VkCommandBuffer>& get_vk_command_buffers() const { return _vk_command_buffers; }

    private:
        std::vector<VkCommandBuffer> _vk_command_buffers;
        std::shared_ptr<logical_device> _logical_device;
        std::shared_ptr<command_pool> _command_pool;
    };
} // namespace owl::vulkan