#pragma once

#include <vulkan/vulkan.h>

#include "logical_device.h"
#include "physical_device.h"
#include "vulkan_object.h"

namespace owl::vulkan
{
    class device_memory : public vulkan_object<VkDeviceMemory>
    {
    public:
        device_memory(const std::shared_ptr<physical_device>& physical_device,
                      const std::shared_ptr<logical_device>& logical_device,
                      const VkMemoryRequirements& memory_requirements,
                      VkMemoryPropertyFlags properties);
        ~device_memory();

    private:
        std::shared_ptr<logical_device> _logical_device;

        uint32_t find_memory_type(const std::shared_ptr<physical_device>& physical_device,
                                  uint32_t type_filter,
                                  VkMemoryPropertyFlags properties);
    };
} // namespace owl::vulkan