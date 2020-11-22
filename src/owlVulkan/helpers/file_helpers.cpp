#include "file_helpers.h"

#include <fstream>
#include <iostream>

namespace owl
{
    std::vector<char> read_file(const std::string& filename)
    {
        std::ifstream file_stream(filename, std::ios::ate | std::ios::binary);

        if (!file_stream.is_open())
        {
            throw std::ios_base::failure("Failed to open file " + filename);
        }

        size_t file_size = (size_t)file_stream.tellg();
        std::vector<char> buffer(file_size);
        file_stream.seekg(0);
        file_stream.read(buffer.data(), file_size);
        file_stream.close();

        return buffer;
    }
}