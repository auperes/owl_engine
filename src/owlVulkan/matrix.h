#pragma once

#include <glm/mat4x4.hpp>

namespace owl::vulkan
{
    struct model_view_projection
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
} // namespace owl::vulkan