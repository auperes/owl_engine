#pragma once

#include <vulkan/vulkan.h>

#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace owl::vulkan
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texture_coordinates;

        bool operator==(const vertex& other) const;
    };

    VkVertexInputBindingDescription get_binding_description();
    std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions();
    VkVertexInputAttributeDescription create_attribute_description(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
} // namespace owl::vulkan

namespace std
{
    template <>
    struct hash<owl::vulkan::vertex>
    {
        size_t operator()(owl::vulkan::vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texture_coordinates) << 1);
        }
    };
} // namespace std