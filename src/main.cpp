#include "engine.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    owl::engine engine;

    try
    {
        engine.run();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Unexpected error occured: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
