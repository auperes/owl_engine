set(HEADERS
    core/buffer.h
    core/command_buffers.h
    core/command_pool.h
    core/debug_messenger.h
    core/descriptor_pool.h
    core/descriptor_set_layout.h
    core/descriptor_sets.h
    core/device_memory.h
    core/fence.h
    core/framebuffer.h
    core/graphics_pipeline.h
    core/image_view.h
    core/image.h
    core/instance.h
    core/logical_device.h
    core/physical_device.h
    core/pipeline_layout.h
    core/pipeline.h
    core/render_pass.h
    core/sampler.h
    core/semaphore.h
    core/shader_module.h
    core/surface.h
    core/swapchain.h
    core/swapchain_support.h
    core/vertex.h
    core/vulkan_object.h
    helpers/file_helpers.h
    helpers/vulkan_collections_helpers.h
    helpers/vulkan_helpers.h
    matrix.h
    queue_families_indices.h)

set(SOURCES
    core/buffer.cpp
    core/command_buffers.cpp
    core/command_pool.cpp
    core/debug_messenger.cpp
    core/descriptor_pool.cpp
    core/descriptor_set_layout.cpp
    core/descriptor_sets.cpp
    core/device_memory.cpp
    core/fence.cpp
    core/framebuffer.cpp
    core/graphics_pipeline.cpp
    core/image_view.cpp
    core/image.cpp
    core/instance.cpp
    core/logical_device.cpp
    core/physical_device.cpp
    core/pipeline_layout.cpp
    core/pipeline.cpp
    core/render_pass.cpp
    core/sampler.cpp
    core/semaphore.cpp
    core/shader_module.cpp
    core/surface.cpp
    core/swapchain.cpp
    core/swapchain_support.cpp
    core/vertex.cpp
    helpers/file_helpers.cpp
    helpers/vulkan_collections_helpers.cpp
    helpers/vulkan_helpers.cpp
    queue_families_indices.cpp)

add_library(owlVulkan SHARED ${SOURCES} ${HEADERS})

target_include_directories(
  owlVulkan
  PRIVATE ${VULKAN_PATH}/Include
  PRIVATE ${GLM_INCLUDE}
  PRIVATE ${PROJECT_SOURCE_DIR}/owlModel
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/core
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/helpers)

add_dependencies(owlVulkan owlModel)
target_link_libraries(
  owlVulkan
  PRIVATE ${VULKAN_PATH}/Lib/vulkan-1.lib)
  # ${CMAKE_CURRENT_BINARY_DIR}/../owlModel/libowlModel.dll.a)
