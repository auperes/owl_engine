#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace owl
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texture_coordinates;

        bool operator==(const vertex& other) const
        {
            return position == other.position && color == other.color && texture_coordinates == other.texture_coordinates;
        }
    };
} // namespace owl

namespace std
{
    template <>
    struct hash<owl::vertex>
    {
        size_t operator()(owl::vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texture_coordinates) << 1);
        }
    };
} // namespace std