#include "engine.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <owlVulkan/helpers/vulkan_collections_helpers.h>
#include <owlVulkan/helpers/vulkan_helpers.h>
#include <owlVulkan/matrix.h>
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

        _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);
    }

    void engine::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
    {
        auto application = reinterpret_cast<engine*>(glfwGetWindowUserPointer(window));
        application->_framebuffer_resized = true;
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

        create_swapchain();
        create_image_views();
        create_render_pass();
        create_descriptor_set_layout();
        create_graphics_pipeline();
        create_depth_resources();
        create_framebuffers();
        _command_pool = std::make_shared<vulkan::command_pool>(_logical_device, _physical_device, _surface);
        create_texture_image();
        create_texture_image_view();
        create_texture_sampler();
        load_model();
        create_buffers();
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers();
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
        auto result = glfwCreateWindowSurface(_instance->get_vk_handle(), _window, nullptr, &vk_surface);
        _surface = std::make_shared<vulkan::surface>(_instance, vk_surface);
        vulkan::helpers::handle_result(result, "Failed to create window surface");
    }

    void engine::load_model()
    {
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        auto success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, model_path.c_str());
        if (!success)
        {
            throw std::runtime_error(warnings + errors);
        }

        std::unordered_map<vulkan::vertex, uint32_t> unique_vertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                vulkan::vertex vertex{};
                vertex.position = {attributes.vertices[3 * index.vertex_index + 0],
                                   attributes.vertices[3 * index.vertex_index + 1],
                                   attributes.vertices[3 * index.vertex_index + 2]};
                vertex.texture_coordinates = {attributes.texcoords[2 * index.texcoord_index + 0],
                                              1.0f - attributes.texcoords[2 * index.texcoord_index + 1]};
                vertex.color = {1.0f, 1.0f, 1.0f};

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(_mesh.vertices.size());
                    _mesh.vertices.push_back(vertex);
                }

                _mesh.indices.push_back(unique_vertices[vertex]);
            }
        }
    }

    void engine::create_buffers()
    {
        _vertex_buffer = vulkan::create_buffer(_mesh.vertices,
                                               _physical_device,
                                               _logical_device,
                                               _command_pool,
                                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        _index_buffer = vulkan::create_buffer(_mesh.indices,
                                              _physical_device,
                                              _logical_device,
                                              _command_pool,
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    void engine::create_uniform_buffers()
    {
        VkDeviceSize buffer_size = sizeof(vulkan::model_view_projection);
        _uniform_buffers.resize(_swapchain->get_vk_swapchain_images().size());

        for (size_t i = 0; i < _swapchain->get_vk_swapchain_images().size(); ++i)
        {
            _uniform_buffers[i] =
                std::make_shared<vulkan::buffer>(_physical_device,
                                                 _logical_device,
                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                 VK_SHARING_MODE_EXCLUSIVE,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                 buffer_size);
        }
    }

    void engine::create_swapchain()
    {
        _swapchain = std::make_shared<vulkan::swapchain>(_physical_device, _logical_device, _surface, WIDTH, HEIGHT);
    }

    void engine::create_descriptor_pool()
    {
        _descriptor_pool = std::make_shared<vulkan::descriptor_pool>(_logical_device, _swapchain->get_vk_swapchain_images().size());
    }

    void engine::create_render_pass()
    {
        _render_pass =
            std::make_shared<vulkan::render_pass>(_physical_device, _logical_device, _swapchain->get_vk_swapchain_image_format());
    }

    void engine::create_graphics_pipeline()
    {
        _pipeline_layout = std::make_shared<vulkan::pipeline_layout>(_logical_device, _descriptor_set_layout);
        _graphics_pipeline = std::make_shared<vulkan::graphics_pipeline>("../build/shaders/passthrough_frag.spv",
                                                                         "../build/shaders/passthrough_vert.spv",
                                                                         _logical_device,
                                                                         _swapchain,
                                                                         _pipeline_layout,
                                                                         _render_pass);
    }

    void engine::create_command_buffers()
    {
        _command_buffers = std::make_shared<vulkan::command_buffers>(_logical_device, _command_pool, _swapchain_framebuffers.size());
        _command_buffers->process_command_buffers(0, [this](const VkCommandBuffer& vk_command_buffer, size_t index) {
            vulkan::process_engine_command_buffer(vk_command_buffer,
                                                  index,
                                                  _swapchain_framebuffers,
                                                  _graphics_pipeline,
                                                  _render_pass,
                                                  _swapchain,
                                                  _vertex_buffer,
                                                  _index_buffer,
                                                  _descriptor_sets,
                                                  _pipeline_layout,
                                                  static_cast<uint32_t>(_mesh.indices.size()));
        });
    }

    void engine::create_descriptor_set_layout()
    {
        _descriptor_set_layout = std::make_shared<vulkan::descriptor_set_layout>(_logical_device);
    }

    void engine::create_descriptor_sets()
    {
        _descriptor_sets = std::make_shared<vulkan::descriptor_sets>(_logical_device,
                                                                     _descriptor_set_layout,
                                                                     _descriptor_pool,
                                                                     _uniform_buffers,
                                                                     _texture_image_view,
                                                                     _texture_sampler,
                                                                     _swapchain->get_vk_swapchain_images().size());
    }

    void engine::create_image_views()
    {
        _swapchain_image_views.reserve(_swapchain->get_vk_swapchain_images().size());
        for (const auto& image : _swapchain->get_vk_swapchain_images())
        {
            _swapchain_image_views.push_back(std::make_shared<vulkan::image_view>(_logical_device,
                                                                                  image,
                                                                                  _swapchain->get_vk_swapchain_image_format(),
                                                                                  VK_IMAGE_ASPECT_COLOR_BIT));
        }
    }

    void engine::create_framebuffers()
    {
        _swapchain_framebuffers.reserve(_swapchain_image_views.size());
        for (const auto& image_view : _swapchain_image_views)
        {
            _swapchain_framebuffers.push_back(
                std::make_shared<vulkan::framebuffer>(image_view, _depth_image_view, _render_pass, _swapchain, _logical_device));
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

    void engine::create_texture_image()
    {
        int texture_width;
        int texture_height;
        int texture_channels;
        stbi_uc* pixels = stbi_load(texture_path.c_str(), &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = texture_width * texture_height * 4;

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image: " + texture_path);
        }

        auto staging_buffer = vulkan::create_staging_buffer(pixels, _physical_device, _logical_device, image_size);

        stbi_image_free(pixels); // TODO not safe if exception occurs

        _texture_image = std::make_shared<vulkan::image>(_physical_device,
                                                         _logical_device,
                                                         static_cast<uint32_t>(texture_width),
                                                         static_cast<uint32_t>(texture_height),
                                                         VK_FORMAT_R8G8B8A8_SRGB,
                                                         VK_IMAGE_TILING_OPTIMAL,
                                                         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        _texture_image->transition_layout(_command_pool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _texture_image->copy_buffer(_command_pool, staging_buffer->get_vk_handle());
        _texture_image->transition_layout(_command_pool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void engine::create_texture_image_view()
    {
        _texture_image_view = std::make_shared<vulkan::image_view>(_logical_device,
                                                                   _texture_image->get_vk_handle(),
                                                                   _texture_image->get_format(),
                                                                   VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void engine::create_texture_sampler() { _texture_sampler = std::make_shared<vulkan::sampler>(_logical_device); }

    void engine::create_depth_resources()
    {
        VkFormat depth_format = vulkan::helpers::find_depth_format(_physical_device);
        _depth_image = std::make_shared<vulkan::image>(_physical_device,
                                                       _logical_device,
                                                       _swapchain->get_vk_swapchain_extent().width,
                                                       _swapchain->get_vk_swapchain_extent().height,
                                                       depth_format,
                                                       VK_IMAGE_TILING_OPTIMAL,
                                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        _depth_image_view =
            std::make_shared<vulkan::image_view>(_logical_device, _depth_image->get_vk_handle(), depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void engine::recreate_swapchain()
    {
        int width = 0;
        int height = 0;

        glfwGetFramebufferSize(_window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(_window, &width, &height);
            glfwWaitEvents();
        }

        _logical_device->wait_idle();
        clean_swapchain();

        create_swapchain();
        create_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_depth_resources();
        create_framebuffers();
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers();
    }

    void engine::clean_swapchain()
    {
        _depth_image_view = nullptr;
        _depth_image = nullptr;
        _swapchain_framebuffers.clear();
        _command_buffers = nullptr;
        _graphics_pipeline = nullptr;
        _pipeline_layout = nullptr;
        _render_pass = nullptr;
        _swapchain_image_views.clear();
        _swapchain = nullptr;
        _uniform_buffers.clear();
        _descriptor_pool = nullptr;
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
        VkResult acquire_result = vkAcquireNextImageKHR(_logical_device->get_vk_handle(),
                                                        _swapchain->get_vk_handle(),
                                                        UINT64_MAX,
                                                        _image_available_semaphores[_current_frame]->get_vk_handle(),
                                                        VK_NULL_HANDLE,
                                                        &image_index);

        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swapchain();
            return;
        }
        else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
        {
            vulkan::helpers::handle_result_error(acquire_result, "Failed to acquire swapchain image.");
        }

        update_uniform_buffers(image_index);

        if (_in_flight_images[image_index] != nullptr)
            _in_flight_images[image_index]->wait_for_fence();

        _in_flight_images[image_index] = _in_flight_fences[_current_frame];

        VkSemaphore wait_semaphores[] = {_image_available_semaphores[_current_frame]->get_vk_handle()};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &_command_buffers->get_vk_command_buffers()[image_index];

        VkSemaphore signal_semaphores[] = {_render_finished_semaphores[_current_frame]->get_vk_handle()};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        vkResetFences(_logical_device->get_vk_handle(), 1, &_in_flight_fences[_current_frame]->get_vk_handle());

        auto result =
            vkQueueSubmit(_logical_device->get_vk_graphics_queue(), 1, &submit_info, _in_flight_fences[_current_frame]->get_vk_handle());
        vulkan::helpers::handle_result(result, "Failed to submit draw command buffer");

        VkPresentInfoKHR presentation_info{};
        presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentation_info.waitSemaphoreCount = 1;
        presentation_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {_swapchain->get_vk_handle()};
        presentation_info.swapchainCount = 1;
        presentation_info.pSwapchains = swapchains;
        presentation_info.pImageIndices = &image_index;
        presentation_info.pResults = nullptr;

        VkResult presentation_result = vkQueuePresentKHR(_logical_device->get_vk_presentation_queue(), &presentation_info);

        if (presentation_result == VK_ERROR_OUT_OF_DATE_KHR || presentation_result == VK_SUBOPTIMAL_KHR || _framebuffer_resized)
        {
            _framebuffer_resized = false;
            recreate_swapchain();
            return;
        }
        else if (presentation_result != VK_SUCCESS)
        {
            vulkan::helpers::handle_result_error(presentation_result, "Failed to acquire swapchain image.");
        }

        _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void engine::update_uniform_buffers(uint32_t current_image)
    {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

        auto extent = _swapchain->get_vk_swapchain_extent();
        vulkan::model_view_projection mvp;
        mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.projection = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
        mvp.projection[1][1] *= -1; // in vulkan Y coordinate is inverted (compared to openGL)

        vulkan::copy_memory(&mvp, _logical_device, *_uniform_buffers[current_image], sizeof(mvp));
    }

    void engine::clean()
    {
        clean_swapchain();

        _texture_sampler = nullptr;
        _texture_image_view = nullptr;
        _texture_image = nullptr;
        _descriptor_set_layout = nullptr;

        _index_buffer = nullptr;
        _vertex_buffer = nullptr;

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
