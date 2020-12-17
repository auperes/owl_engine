#pragma once

#include <memory>
#include <string>
#include <vector>

#include <buffer.h>
#include <command_buffers.h>
#include <command_pool.h>
#include <debug_messenger.h>
#include <descriptor_pool.h>
#include <descriptor_set_layout.h>
#include <descriptor_sets.h>
#include <fence.h>
#include <framebuffer.h>
#include <graphics_pipeline.h>
#include <image.h>
#include <image_view.h>
#include <instance.h>
#include <logical_device.h>
#include <mesh.h>
#include <physical_device.h>
#include <pipeline_layout.h>
#include <render_pass.h>
#include <sampler.h>
#include <semaphore.h>
#include <surface.h>
#include <swapchain.h>
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

        const vulkan::instance& get_instance() { return *_instance; }

        void create_instance(std::vector<const char*>&& extensions);
        void set_vk_surface(VkSurfaceKHR vk_surface);
        void initialize(uint32_t width, uint32_t height, mesh&& mesh, texture&& texture);

        bool acquire_image();
        bool draw_image();
        void recreate_swapchain(uint32_t width, uint32_t height);

        void wait_idle();

        void set_framebuffer_resized(bool is_resized) { _framebuffer_resized = is_resized; }

    private:
        std::shared_ptr<vulkan::instance> _instance;
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
        std::shared_ptr<vulkan::descriptor_set_layout> _descriptor_set_layout;
        std::shared_ptr<vulkan::descriptor_pool> _descriptor_pool;
        std::shared_ptr<vulkan::descriptor_sets> _descriptor_sets;

        std::shared_ptr<vulkan::buffer> _vertex_buffer;
        std::shared_ptr<vulkan::buffer> _index_buffer;
        std::vector<std::shared_ptr<vulkan::buffer>> _uniform_buffers;

        std::vector<std::shared_ptr<vulkan::semaphore>> _image_available_semaphores;
        std::vector<std::shared_ptr<vulkan::semaphore>> _render_finished_semaphores;

        std::vector<std::shared_ptr<vulkan::fence>> _in_flight_fences;
        std::vector<std::shared_ptr<vulkan::fence>> _in_flight_images;

        std::shared_ptr<vulkan::image> _texture_image;
        std::shared_ptr<vulkan::image_view> _texture_image_view;
        std::shared_ptr<vulkan::sampler> _texture_sampler;

        std::shared_ptr<vulkan::image> _depth_image;
        std::shared_ptr<vulkan::image_view> _depth_image_view;

        std::shared_ptr<vulkan::image> _color_image;
        std::shared_ptr<vulkan::image_view> _color_image_view;

        uint32_t _mip_levels;
        VkSampleCountFlagBits _msaa_samples = VK_SAMPLE_COUNT_1_BIT;

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
        void create_image_views();
        void create_framebuffers();
        void create_synchronization_objects();
        void create_texture_resources(texture&& texture);
        void create_depth_resources();
        void create_color_resources();

        void clean_swapchain();

        void run_internal();

        void update_uniform_buffers(uint32_t current_image);
    };
} // namespace owl