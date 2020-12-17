#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "physical_device.h"
#include "surface.h"
#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class command_buffers;

    class logical_device : public vulkan_object<VkDevice>
    {
    public:
        logical_device(const std::shared_ptr<physical_device>& physical_device,
                       const std::shared_ptr<surface>& surface,
                       const std::vector<const char*>& device_extensions,
                       const std::vector<const char*>& validation_layers,
                       bool enable_validation_layers);
        ~logical_device();

        const VkQueue& get_vk_graphics_queue() const { return _vk_graphics_queue; }
        const VkQueue& get_vk_presentation_queue() const { return _vk_presentation_queue; }

        void wait_idle();
        void submit_to_graphics_queue(const command_buffers& command_buffers);

    private:
        VkQueue _vk_graphics_queue;
        VkQueue _vk_presentation_queue;
    };
} // namespace owl::vulkan