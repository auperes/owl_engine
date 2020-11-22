#include "engine.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>

#include <owlVulkan/helpers/vulkan_collections_helpers.h>
#include <owlVulkan/helpers/vulkan_helpers.h>
#include <owlVulkan/queue_families_indices.h>
#include <owlVulkan/swapchain.h>

namespace owl
{
    engine::engine() {}

    void engine::run()
    {
        initialize();
        run_internal();
        clean();
    }

    void engine::initialize()
    {
        initialize_window();
        initialize_vulkan();
    }

    void engine::initialize_window()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void engine::initialize_vulkan()
    {
        auto glfw_extensions = get_required_extensions();
        _instance = std::make_shared<vulkan::instance>(enable_validation_layers, validation_layers, glfw_extensions);

        display_available_extensions();

        if (enable_validation_layers)
            _debug_messenger = std::make_unique<vulkan::debug_messenger>(_instance);

        create_surface();

        _physical_device = std::make_shared<vulkan::physical_device>(_instance, _surface, device_extensions);
        _logical_device = std::make_shared<vulkan::logical_device>(_physical_device,
                                                                   _surface,
                                                                   device_extensions,
                                                                   validation_layers,
                                                                   enable_validation_layers);
        _command_pool = std::make_shared<vulkan::command_pool>(_logical_device, _physical_device, _surface);
        create_swapchain();
        create_synchronization_objects();
    }

    std::vector<const char*> engine::get_required_extensions()
    {
        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extensions_count);

        if (enable_validation_layers)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    void engine::display_available_extensions()
    {
        std::vector<VkExtensionProperties> extensions = vulkan::helpers::get_instance_extension_properties();

        std::cout << "Available extensions:" << std::endl;
        for (const auto& extension : extensions)
            std::cout << "\t" << extension.extensionName << std::endl;
    }

    void engine::create_surface()
    {
        VkSurfaceKHR vk_surface;
        auto result = glfwCreateWindowSurface(_instance->get_vk_instance(), _window, nullptr, &vk_surface);
        _surface = std::make_shared<vulkan::surface>(_instance, vk_surface);
        vulkan::helpers::handle_result(result, "Failed to create window surface");
    }

    void engine::create_swapchain()
    {
        _swapchain = std::make_shared<vulkan::swapchain>(_physical_device, _logical_device, _surface, WIDTH, HEIGHT);
        create_image_views();

        _render_pass = std::make_shared<vulkan::render_pass>(_logical_device, _swapchain->get_vk_swapchain_image_format());

        _pipeline_layout = std::make_shared<vulkan::pipeline_layout>(_logical_device);
        _graphics_pipeline = std::make_shared<vulkan::graphics_pipeline>("../build/shaders/passthrough_frag.spv",
                                                                         "../build/shaders/passthrough_vert.spv",
                                                                         _logical_device,
                                                                         _swapchain,
                                                                         _pipeline_layout,
                                                                         _render_pass);

        create_framebuffers();

        _command_buffers = std::make_shared<vulkan::command_buffers>(_logical_device,
                                                                     _command_pool,
                                                                     _swapchain_framebuffers,
                                                                     _graphics_pipeline,
                                                                     _render_pass,
                                                                     _swapchain);
    }

    void engine::create_image_views()
    {
        _swapchain_image_views.reserve(_swapchain->get_vk_swapchain_images().size());
        for (size_t i = 0; i < _swapchain->get_vk_swapchain_images().size(); ++i)
        {
            // TODO
            auto& image = _swapchain->get_vk_swapchain_images()[i];
            _swapchain_image_views.push_back(std::make_shared<vulkan::image_view>(_logical_device, _swapchain, i));
        }
    }

    void engine::create_framebuffers()
    {
        _swapchain_framebuffers.reserve(_swapchain_image_views.size());
        for (const auto& image_view : _swapchain_image_views)
        {
            _swapchain_framebuffers.push_back(std::make_shared<vulkan::framebuffer>(image_view, _render_pass, _swapchain, _logical_device));
        }
    }

    void engine::create_synchronization_objects()
    {
        _image_available_semaphores.reserve(MAX_FRAMES_IN_FLIGHT);
        _render_finished_semaphores.reserve(MAX_FRAMES_IN_FLIGHT);

        _in_flight_fences.reserve(MAX_FRAMES_IN_FLIGHT);
        _in_flight_images.resize(_swapchain->get_vk_swapchain_images().size(), nullptr);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            _image_available_semaphores.push_back(std::make_shared<vulkan::semaphore>(_logical_device));
            _render_finished_semaphores.push_back(std::make_shared<vulkan::semaphore>(_logical_device));

            _in_flight_fences.push_back(std::make_shared<vulkan::fence>(_logical_device));
        }
    }

    void engine::recreate_swapchain()
    {
        _logical_device->wait_idle();
        clean_swapchain();
        create_swapchain();
    }

    void engine::clean_swapchain()
    {
        _swapchain_framebuffers.clear();
        _command_buffers = nullptr;
        _graphics_pipeline = nullptr;
        _pipeline_layout = nullptr;
        _render_pass = nullptr;
        _swapchain_image_views.clear();
        _swapchain = nullptr;
    }

    void engine::run_internal()
    {
        while (!glfwWindowShouldClose(_window))
        {
            glfwPollEvents();
            draw_frame();
        }

        _logical_device->wait_idle();
    }

    void engine::draw_frame()
    {
        _in_flight_fences[_current_frame]->wait_for_fence();

        uint32_t image_index;
        vkAcquireNextImageKHR(_logical_device->get_vk_device(),
                              _swapchain->get_vk_swapchain(),
                              UINT64_MAX,
                              _image_available_semaphores[_current_frame]->get_vk_semaphore(),
                              VK_NULL_HANDLE,
                              &image_index);

        if (_in_flight_images[image_index] != nullptr)
            _in_flight_images[image_index]->wait_for_fence();

        _in_flight_images[image_index] = _in_flight_fences[_current_frame];

        VkSemaphore wait_semaphores[] = {_image_available_semaphores[_current_frame]->get_vk_semaphore()};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &_command_buffers->get_vk_command_buffers()[image_index];

        VkSemaphore signal_semaphores[] = {_render_finished_semaphores[_current_frame]->get_vk_semaphore()};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        vkResetFences(_logical_device->get_vk_device(), 1, &_in_flight_fences[_current_frame]->get_vk_fence());

        auto result =
            vkQueueSubmit(_logical_device->get_vk_graphics_queue(), 1, &submit_info, _in_flight_fences[_current_frame]->get_vk_fence());
        vulkan::helpers::handle_result(result, "Failed to submit draw command buffer");

        VkPresentInfoKHR presentation_info{};
        presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentation_info.waitSemaphoreCount = 1;
        presentation_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {_swapchain->get_vk_swapchain()};
        presentation_info.swapchainCount = 1;
        presentation_info.pSwapchains = swapchains;
        presentation_info.pImageIndices = &image_index;
        presentation_info.pResults = nullptr;

        vkQueuePresentKHR(_logical_device->get_vk_presentation_queue(), &presentation_info);
        vkQueueWaitIdle(_logical_device->get_vk_presentation_queue());

        _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void engine::clean()
    {
        clean_swapchain();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            _in_flight_fences.clear();
            _render_finished_semaphores.clear();
            _image_available_semaphores.clear();
        }

        _command_pool == nullptr;
        _logical_device = nullptr;
        _debug_messenger = nullptr;
        _surface = nullptr;
        _instance = nullptr;

        glfwDestroyWindow(_window);

        glfwTerminate();
    }
} // namespace owl
