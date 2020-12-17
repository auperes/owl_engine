#pragma once

#include <vulkan/vulkan.h>

#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace owl::vulkan::core
{
    VkVertexInputBindingDescription get_binding_description();
    std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions();
    VkVertexInputAttributeDescription create_attribute_description(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
} // namespace owl::vulkan::core