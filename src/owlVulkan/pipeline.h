#pragma once

#include <vulkan/vulkan.h>

namespace owl::vulkan
{
    class pipeline
    {
    public:
        pipeline();
        ~pipeline();

        const VkPipeline& get_vk_pipeline() const { return _vk_pipeline; }

    protected:
        VkPipeline _vk_pipeline;
    };
} // namespace owl::vulkan