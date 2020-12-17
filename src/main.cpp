#include "vulkan_window.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        owl::vulkan_window window(800, 600);
        window.run();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Unexpected error occured: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
