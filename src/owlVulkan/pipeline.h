#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_object.h"

namespace owl::vulkan
{
    class pipeline : public vulkan_object<VkPipeline>
    {
    public:
        pipeline();
        ~pipeline();

    protected:
    };
} // namespace owl::vulkan