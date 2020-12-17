#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include <mesh.h>
#include <texture.h>

#include "vulkan_engine.h"

namespace owl
{
    class vulkan_window
    {
    public:
        vulkan_window(const uint32_t width, const uint32_t height);
        ~vulkan_window();

        void run();

        static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

    private:
        const std::string model_path = "resources/models/viking_room.obj";
        const std::string texture_path = "resources/textures/viking_room.png";

        GLFWwindow* _window;
        std::unique_ptr<vulkan_engine> _engine;
        bool _framebuffer_resized = false;

        texture load_image(const std::string& path);
        mesh load_model(const std::string& path);
    };
} // namespace owl