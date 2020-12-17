#include "vulkan_engine.h"

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

#include <core/swapchain.h>
#include <helpers/vulkan_collections_helpers.h>
#include <helpers/vulkan_helpers.h>
#include <matrix.h>
#include <queue_families_indices.h>

namespace owl
{
    vulkan_engine::vulkan_engine() {}

    vulkan_engine::~vulkan_engine()
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

        _pipeline_layout = nullptr;
        _graphics_pipeline = nullptr;
        _command_pool == nullptr;
        _logical_device = nullptr;
        _surface = nullptr;
        _instance = nullptr;
    }

    void vulkan_engine::create_instance(std::vector<const char*>&& extensions)
    {
        if (enable_validation_layers)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        _instance = std::make_shared<vulkan::core::instance>(enable_validation_layers, validation_layers, extensions);

        display_available_extensions();
    }

    void vulkan_engine::set_vk_surface(VkSurfaceKHR vk_surface)
    {
        _surface = std::make_shared<vulkan::core::surface>(_instance, vk_surface);
    }

    void vulkan_engine::initialize(uint32_t width, uint32_t height, mesh&& mesh, texture&& texture)
    {
        _physical_device = std::make_shared<vulkan::core::physical_device>(_instance, _surface, device_extensions);
        _logical_device = std::make_shared<vulkan::core::logical_device>(_physical_device,
                                                                         _surface,
                                                                         device_extensions,
                                                                         validation_layers,
                                                                         enable_validation_layers);

        auto indices = _physical_device->find_queue_families();
        _command_pool = std::make_shared<vulkan::core::command_pool>(_logical_device, _surface, indices.graphics_family.value());

        create_swapchain(width, height); // swapchain
        create_render_pass();            // swapchain
        _swapchain->create_framebuffers(_render_pass);
        create_texture_resources(std::move(texture)); // TODO merge with image view // need command pool

        _indices_size = static_cast<uint32_t>(mesh.indices.size());
        create_buffers(std::move(mesh)); // use mesh // need command pool

        create_uniform_buffers(); // swapchain
        create_descriptor_pool(); // swapchain

        _descriptor_set_layout = std::make_shared<vulkan::core::descriptor_set_layout>(_logical_device);
        _pipeline_layout = std::make_shared<vulkan::core::pipeline_layout>(_logical_device, _descriptor_set_layout);
        _graphics_pipeline = std::make_shared<vulkan::core::graphics_pipeline>("../build/shaders/passthrough_frag.spv",
                                                                               "../build/shaders/passthrough_vert.spv",
                                                                               _logical_device,
                                                                               _swapchain,
                                                                               _pipeline_layout,
                                                                               _render_pass,
                                                                               _physical_device->get_max_usable_sample_count());

        create_descriptor_sets();              // swapchain // need descriptor_set_layout
        create_command_buffers(_indices_size); // swapchain // need pipeline_layout, graphics_pipeline, command_pool

        create_synchronization_objects();
    }

    bool vulkan_engine::acquire_image()
    {
        _in_flight_fences[_current_frame]->wait_for_fence();

        VkResult acquire_result = vkAcquireNextImageKHR(_logical_device->get_vk_handle(),
                                                        _swapchain->get_vk_handle(),
                                                        UINT64_MAX,
                                                        _image_available_semaphores[_current_frame]->get_vk_handle(),
                                                        VK_NULL_HANDLE,
                                                        &_current_image_index);

        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
            return false;
        else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
            vulkan::helpers::handle_result_error(acquire_result, "Failed to acquire swapchain image.");

        return true;
    }

    bool vulkan_engine::draw_image()
    {
        update_uniform_buffers(_current_image_index);

        if (_in_flight_images[_current_image_index] != nullptr)
            _in_flight_images[_current_image_index]->wait_for_fence();

        _in_flight_images[_current_image_index] = _in_flight_fences[_current_frame];

        VkSemaphore wait_semaphores[] = {_image_available_semaphores[_current_frame]->get_vk_handle()};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &_command_buffers->get_vk_command_buffers()[_current_image_index];

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
        presentation_info.pImageIndices = &_current_image_index;
        presentation_info.pResults = nullptr;

        VkResult presentation_result = vkQueuePresentKHR(_logical_device->get_vk_presentation_queue(), &presentation_info);

        if (presentation_result == VK_ERROR_OUT_OF_DATE_KHR || presentation_result == VK_SUBOPTIMAL_KHR || _framebuffer_resized)
        {
            _framebuffer_resized = false;
            return false;
        }
        else if (presentation_result != VK_SUCCESS)
        {
            vulkan::helpers::handle_result_error(presentation_result, "Failed to acquire swapchain image.");
        }

        _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

        return true;
    }

    void vulkan_engine::wait_idle() { _logical_device->wait_idle(); }

    void vulkan_engine::display_available_extensions()
    {
        std::vector<VkExtensionProperties> extensions = vulkan::helpers::get_instance_extension_properties();

        std::cout << "Available extensions:" << std::endl;
        for (const auto& extension : extensions)
            std::cout << "\t" << extension.extensionName << std::endl;
    }

    void vulkan_engine::create_buffers(mesh&& mesh)
    {
        _vertex_buffer = vulkan::core::create_buffer(mesh.vertices,
                                                     _physical_device,
                                                     _logical_device,
                                                     _command_pool,
                                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        _index_buffer = vulkan::core::create_buffer(mesh.indices,
                                                    _physical_device,
                                                    _logical_device,
                                                    _command_pool,
                                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    void vulkan_engine::create_uniform_buffers()
    {
        VkDeviceSize buffer_size = sizeof(vulkan::model_view_projection);
        _uniform_buffers.resize(_swapchain->get_vk_images().size());

        for (size_t i = 0; i < _swapchain->get_vk_images().size(); ++i)
        {
            _uniform_buffers[i] =
                std::make_shared<vulkan::core::buffer>(_physical_device,
                                                       _logical_device,
                                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                       VK_SHARING_MODE_EXCLUSIVE,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                       buffer_size);
        }
    }

    void vulkan_engine::create_swapchain(uint32_t width, uint32_t height)
    {
        _swapchain = std::make_shared<vulkan::core::swapchain>(_physical_device, _logical_device, _surface, _render_pass, width, height);
    }

    void vulkan_engine::create_descriptor_pool()
    {
        _descriptor_pool = std::make_shared<vulkan::core::descriptor_pool>(_logical_device, _swapchain->get_vk_images().size());
    }

    void vulkan_engine::create_render_pass()
    {
        auto depth_format = _physical_device->get_depth_format();
        auto color_format = _swapchain->get_vk_image_format();

        _render_pass = std::make_shared<vulkan::core::render_pass>(_logical_device,
                                                                   color_format,
                                                                   depth_format,
                                                                   _physical_device->get_max_usable_sample_count());
    }

    void vulkan_engine::create_command_buffers(uint32_t indices_size)
    {
        _command_buffers =
            std::make_shared<vulkan::core::command_buffers>(_logical_device, _command_pool, _swapchain->get_framebuffers().size());
        _command_buffers->process_command_buffers(0, [this, indices_size](const VkCommandBuffer& vk_command_buffer, size_t index) {
            vulkan::core::process_engine_command_buffer(vk_command_buffer,
                                                        index,
                                                        _graphics_pipeline,
                                                        _render_pass,
                                                        _swapchain,
                                                        _vertex_buffer,
                                                        _index_buffer,
                                                        _descriptor_sets,
                                                        _pipeline_layout,
                                                        indices_size);
        });
    }

    void vulkan_engine::create_descriptor_sets()
    {
        _descriptor_sets = std::make_shared<vulkan::core::descriptor_sets>(_logical_device,
                                                                           _descriptor_set_layout,
                                                                           _descriptor_pool,
                                                                           _uniform_buffers,
                                                                           _texture_image_view,
                                                                           _texture_sampler,
                                                                           _swapchain->get_vk_images().size());
    }

    void vulkan_engine::create_synchronization_objects()
    {
        _image_available_semaphores.reserve(MAX_FRAMES_IN_FLIGHT);
        _render_finished_semaphores.reserve(MAX_FRAMES_IN_FLIGHT);

        _in_flight_fences.reserve(MAX_FRAMES_IN_FLIGHT);
        _in_flight_images.resize(_swapchain->get_vk_images().size(), nullptr);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            _image_available_semaphores.push_back(std::make_shared<vulkan::core::semaphore>(_logical_device));
            _render_finished_semaphores.push_back(std::make_shared<vulkan::core::semaphore>(_logical_device));

            _in_flight_fences.push_back(std::make_shared<vulkan::core::fence>(_logical_device));
        }
    }

    void vulkan_engine::create_texture_resources(texture&& texture)
    {
        _mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture.width, texture.height))));
        VkDeviceSize image_size = texture.width * texture.height * 4;

        auto staging_buffer = vulkan::core::create_staging_buffer(texture.data.data(), _physical_device, _logical_device, image_size);

        _texture_image = std::make_shared<vulkan::core::image>(_physical_device,
                                                               _logical_device,
                                                               static_cast<uint32_t>(texture.width),
                                                               static_cast<uint32_t>(texture.height),
                                                               _mip_levels,
                                                               VK_SAMPLE_COUNT_1_BIT,
                                                               VK_FORMAT_R8G8B8A8_SRGB,
                                                               VK_IMAGE_TILING_OPTIMAL,
                                                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                   VK_IMAGE_USAGE_SAMPLED_BIT,
                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        _texture_image->transition_layout(_command_pool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _texture_image->copy_buffer(_command_pool, staging_buffer->get_vk_handle());

        if (_physical_device->supports_linear_filtering(_texture_image->get_format()))
            _texture_image->generate_mipmaps(_command_pool);

        _texture_image_view = std::make_shared<vulkan::core::image_view>(_logical_device,
                                                                         _texture_image->get_vk_handle(),
                                                                         _mip_levels,
                                                                         _texture_image->get_format(),
                                                                         VK_IMAGE_ASPECT_COLOR_BIT);

        _texture_sampler = std::make_shared<vulkan::core::sampler>(_logical_device, _mip_levels);
    }

    void vulkan_engine::recreate_swapchain(uint32_t width, uint32_t height)
    {
        _logical_device->wait_idle();
        clean_swapchain();

        create_swapchain(width, height);
        create_render_pass();
        _swapchain->create_framebuffers(_render_pass);
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers(_indices_size);
    }

    void vulkan_engine::clean_swapchain()
    {
        _command_buffers = nullptr;
        _render_pass = nullptr;
        _swapchain = nullptr;
        _uniform_buffers.clear();
        _descriptor_pool = nullptr;
    }

    void vulkan_engine::update_uniform_buffers(uint32_t current_image)
    {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

        auto extent = _swapchain->get_vk_extent();
        vulkan::model_view_projection mvp;
        mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.projection = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
        mvp.projection[1][1] *= -1; // in vulkan Y coordinate is inverted (compared to openGL)

        vulkan::core::copy_memory(&mvp, _logical_device, *_uniform_buffers[current_image], sizeof(mvp));
    }
} // namespace owl
