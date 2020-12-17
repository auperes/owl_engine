#include "vulkan_window.h"

#include <unordered_map>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace owl
{
    vulkan_window::vulkan_window(const uint32_t width, const uint32_t height)
        : _engine(std::make_unique<vulkan_engine>())
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        _window = glfwCreateWindow(width, height, "Owl Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);

        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extensions_count);

        _engine->create_instance(std::move(extensions));

        VkSurfaceKHR vk_surface;
        auto result = glfwCreateWindowSurface(_engine->get_instance().get_vk_handle(), _window, nullptr, &vk_surface);
        vulkan::helpers::handle_result(result, "Failed to create window surface");
        _engine->set_vk_surface(vk_surface);

        auto mesh = load_model(model_path);
        auto texture = load_image(texture_path);

        _engine->initialize(width, height, std::move(mesh), std::move(texture));
    }

    vulkan_window::~vulkan_window()
    {
        _engine = nullptr;
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void vulkan_window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
    {
        auto application = reinterpret_cast<vulkan_window*>(glfwGetWindowUserPointer(window));
        application->_engine->set_framebuffer_resized(true);
    }

    void vulkan_window::run()
    {
        while (!glfwWindowShouldClose(_window))
        {
            glfwPollEvents();
            auto success = _engine->acquire_image();
            if (success)
                success &= _engine->draw_image();

            if (!success)
            {
                int width = 0;
                int height = 0;

                glfwGetFramebufferSize(_window, &width, &height);
                while (width == 0 || height == 0)
                {
                    glfwGetFramebufferSize(_window, &width, &height);
                    glfwWaitEvents();
                }

                _engine->recreate_swapchain(width, height);
            }
        }

        _engine->wait_idle();
    }

    texture vulkan_window::load_image(const std::string& path)
    {
        texture texture;
        stbi_uc* pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, STBI_rgb_alpha);

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image: " + path);
        }

        texture.data = std::vector<unsigned char>(pixels, pixels + texture.width * texture.height * 4);

        return texture;
    }

    mesh vulkan_window::load_model(const std::string& path)
    {
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        auto success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, path.c_str());
        if (!success)
        {
            throw std::runtime_error(warnings + errors);
        }

        std::unordered_map<vertex, uint32_t> unique_vertices{};
        mesh mesh;

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                vertex vertex{};
                vertex.position = {attributes.vertices[3 * index.vertex_index + 0],
                                   attributes.vertices[3 * index.vertex_index + 1],
                                   attributes.vertices[3 * index.vertex_index + 2]};
                vertex.texture_coordinates = {attributes.texcoords[2 * index.texcoord_index + 0],
                                              1.0f - attributes.texcoords[2 * index.texcoord_index + 1]};
                vertex.color = {1.0f, 1.0f, 1.0f};

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(vertex);
                }

                mesh.indices.push_back(unique_vertices[vertex]);
            }
        }

        return mesh;
    }
} // namespace owl