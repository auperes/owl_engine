#pragma once

#include <vulkan/vulkan.h>

#include <optional>

namespace owl::vulkan
{
    struct queue_families_indices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> presentation_family;

        bool is_complete();
    };
} // namespace owl::vulkan
