#pragma once

#include <memory>
#include <string>
#include <vector>

#include <core/buffer.h>
#include <core/command_buffers.h>
#include <core/command_pool.h>
#include <core/debug_messenger.h>
#include <core/descriptor_pool.h>
#include <core/descriptor_set_layout.h>
#include <core/descriptor_sets.h>
#include <core/fence.h>
#include <core/framebuffer.h>
#include <core/graphics_pipeline.h>
#include <core/image.h>
#include <core/image_view.h>
#include <core/instance.h>
#include <core/logical_device.h>
#include <core/physical_device.h>
#include <core/pipeline_layout.h>
#include <core/render_pass.h>
#include <core/sampler.h>
#include <core/semaphore.h>
#include <core/surface.h>
#include <core/swapchain.h>
#include <mesh.h>
#include <texture.h>

namespace owl
{
    class vulkan_engine
    {
    public:
        const int MAX_FRAMES_IN_FLIGHT = 2;

        const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
        const bool enable_validation_layers = false;
#else
        const bool enable_validation_layers = true;
#endif

        vulkan_engine();
        ~vulkan_engine();

        const vulkan::core::instance& get_instance() { return *_instance; }

        void create_instance(std::vector<const char*>&& extensions);
        void set_vk_surface(VkSurfaceKHR vk_surface);
        void initialize(uint32_t width, uint32_t height, mesh&& mesh, texture&& texture);

        bool acquire_image();
        bool draw_image();
        void recreate_swapchain(uint32_t width, uint32_t height);

        void wait_idle();

        void set_framebuffer_resized(bool is_resized) { _framebuffer_resized = is_resized; }

    private:
        std::shared_ptr<vulkan::core::instance> _instance;
        std::shared_ptr<vulkan::core::surface> _surface;
        std::shared_ptr<vulkan::core::physical_device> _physical_device;
        std::shared_ptr<vulkan::core::logical_device> _logical_device;

        std::shared_ptr<vulkan::core::swapchain> _swapchain;
        std::shared_ptr<vulkan::core::render_pass> _render_pass;
        std::shared_ptr<vulkan::core::pipeline_layout> _pipeline_layout;
        std::shared_ptr<vulkan::core::graphics_pipeline> _graphics_pipeline;
        std::shared_ptr<vulkan::core::command_pool> _command_pool;
        std::shared_ptr<vulkan::core::command_buffers> _command_buffers;
        std::shared_ptr<vulkan::core::descriptor_set_layout> _descriptor_set_layout;
        std::shared_ptr<vulkan::core::descriptor_pool> _descriptor_pool;
        std::shared_ptr<vulkan::core::descriptor_sets> _descriptor_sets;

        std::shared_ptr<vulkan::core::buffer> _vertex_buffer;
        std::shared_ptr<vulkan::core::buffer> _index_buffer;
        std::vector<std::shared_ptr<vulkan::core::buffer>> _uniform_buffers;

        std::vector<std::shared_ptr<vulkan::core::semaphore>> _image_available_semaphores;
        std::vector<std::shared_ptr<vulkan::core::semaphore>> _render_finished_semaphores;

        std::vector<std::shared_ptr<vulkan::core::fence>> _in_flight_fences;
        std::vector<std::shared_ptr<vulkan::core::fence>> _in_flight_images;

        std::shared_ptr<vulkan::core::image> _texture_image;
        std::shared_ptr<vulkan::core::image_view> _texture_image_view;
        std::shared_ptr<vulkan::core::sampler> _texture_sampler;

        uint32_t _mip_levels;

        size_t _current_frame = 0;
        uint32_t _current_image_index = 0;
        bool _framebuffer_resized = false;

        uint32_t _indices_size = 0;

        void display_available_extensions();

        void create_buffers(mesh&& mesh);
        void create_uniform_buffers();
        void create_swapchain(uint32_t width, uint32_t height);
        void create_descriptor_pool();
        void create_render_pass();
        void create_command_buffers(uint32_t indices_size);
        void create_descriptor_sets();
        void create_synchronization_objects();
        void create_texture_resources(texture&& texture);

        void clean_swapchain();

        void run_internal();

        void update_uniform_buffers(uint32_t current_image);
    };
} // namespace owl