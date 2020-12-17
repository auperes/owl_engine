#pragma once

#include <vector>

#include "vertex.h"

namespace owl
{
    struct mesh
    {
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;
    };
}