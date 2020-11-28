#pragma once

#include <vector>

#include "vertex.h"

namespace owl::vulkan
{
    struct mesh
    {
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;
    };
}