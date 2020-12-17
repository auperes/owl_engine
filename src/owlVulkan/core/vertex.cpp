#include "vertex.h"

#include <vertex.h>

namespace owl::vulkan::core
{
    VkVertexInputBindingDescription get_binding_description()
    {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};
        attribute_descriptions[0] = create_attribute_description(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, position));
        attribute_descriptions[1] = create_attribute_description(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color));
        attribute_descriptions[2] = create_attribute_description(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, texture_coordinates));

        return attribute_descriptions;
    }

    VkVertexInputAttributeDescription create_attribute_description(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        VkVertexInputAttributeDescription attribute_description{};
        attribute_description.binding = binding;
        attribute_description.location = location;
        attribute_description.format = format;
        attribute_description.offset = offset;

        return attribute_description;
    }
} // namespace owl::vulkan::core