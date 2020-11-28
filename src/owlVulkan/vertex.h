#pragma once

#include <vulkan/vulkan.h>

#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace owl::vulkan
{
    struct vertex
    {
        glm::vec2 position;
        glm::vec3 color;
        glm::vec2 texture_coordinates;
    };

    VkVertexInputBindingDescription get_binding_description();
    std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions();
    VkVertexInputAttributeDescription create_attribute_description(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
}