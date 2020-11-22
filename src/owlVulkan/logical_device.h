#pragma once

#include <vulkan/vulkan.h>

#include "physical_device.h"
#include "surface.h"

namespace owl::vulkan
{
    class logical_device
    {
    public:
        logical_device(const std::shared_ptr<physical_device>& physical_device,
                       const std::shared_ptr<surface>& surface,
                       const std::vector<const char*>& device_extensions,
                       const std::vector<const char*>& validation_layers,
                       bool enable_validation_layers);
        ~logical_device();

        const VkDevice& get_vk_device() const { return _vk_device; }
        const VkQueue& get_vk_graphics_queue() const { return _vk_graphics_queue; }
        const VkQueue& get_vk_presentation_queue() const { return _vk_presentation_queue; }

        void wait_idle();

    private:
        VkDevice _vk_device;

        VkQueue _vk_graphics_queue;
        VkQueue _vk_presentation_queue;
    };
} // namespace owl::vulkan