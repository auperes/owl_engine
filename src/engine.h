#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include <owlVulkan/command_buffers.h>
#include <owlVulkan/command_pool.h>
#include <owlVulkan/debug_messenger.h>
#include <owlVulkan/fence.h>
#include <owlVulkan/framebuffer.h>
#include <owlVulkan/graphics_pipeline.h>
#include <owlVulkan/image_view.h>
#include <owlVulkan/instance.h>
#include <owlVulkan/logical_device.h>
#include <owlVulkan/physical_device.h>
#include <owlVulkan/pipeline_layout.h>
#include <owlVulkan/render_pass.h>
#include <owlVulkan/semaphore.h>
#include <owlVulkan/surface.h>
#include <owlVulkan/swapchain.h>

namespace owl
{
    class engine
    {
    public:
        engine();

        void run();

    private:
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
        const int MAX_FRAMES_IN_FLIGHT = 2;
        const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
        const bool enable_validation_layers = false;
#else
        const bool enable_validation_layers = true;
#endif

        GLFWwindow* _window;

        std::shared_ptr<vulkan::instance> _instance;
        std::unique_ptr<vulkan::debug_messenger> _debug_messenger;
        std::shared_ptr<vulkan::surface> _surface;
        std::shared_ptr<vulkan::physical_device> _physical_device;
        std::shared_ptr<vulkan::logical_device> _logical_device;
        std::shared_ptr<vulkan::swapchain> _swapchain;
        std::vector<std::shared_ptr<vulkan::image_view>> _swapchain_image_views;
        std::shared_ptr<vulkan::render_pass> _render_pass;
        std::shared_ptr<vulkan::pipeline_layout> _pipeline_layout;
        std::shared_ptr<vulkan::graphics_pipeline> _graphics_pipeline;
        std::vector<std::shared_ptr<vulkan::framebuffer>> _swapchain_framebuffers;
        std::shared_ptr<vulkan::command_pool> _command_pool;
        std::shared_ptr<vulkan::command_buffers> _command_buffers;

        std::vector<std::shared_ptr<vulkan::semaphore>> _image_available_semaphores;
        std::vector<std::shared_ptr<vulkan::semaphore>> _render_finished_semaphores;

        std::vector<std::shared_ptr<vulkan::fence>> _in_flight_fences;
        std::vector<std::shared_ptr<vulkan::fence>> _in_flight_images;

        size_t _current_frame = 0;

        void initialize();
        void initialize_window();
        void initialize_vulkan();

        std::vector<const char*> get_required_extensions();
        void display_available_extensions();

        void create_surface();
        void create_swapchain();
        void create_image_views();
        void create_framebuffers();
        void create_synchronization_objects();

        void recreate_swapchain();

        void clean_swapchain();

        void run_internal();
        void draw_frame();

        void clean();
    };
} // namespace owl
