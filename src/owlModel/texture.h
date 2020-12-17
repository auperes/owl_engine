#pragma once

#include <vector>

namespace owl
{
    struct texture
    {
        std::vector<unsigned char> data;
        int width;
        int height;
        int channels;
    };
}