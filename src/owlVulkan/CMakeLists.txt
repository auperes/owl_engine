set(HEADERS
    command_buffers.h
    command_pool.h
    debug_messenger.h
    fence.h
    framebuffer.h
    graphics_pipeline.h
    helpers/file_helpers.h
    helpers/vulkan_collections_helpers.h
    helpers/vulkan_helpers.h
    image_view.h
    instance.h
    logical_device.h
    physical_device.h
    pipeline_layout.h
    pipeline.h
    queue_families_indices.h
    render_pass.h
    semaphore.h
    shader_module.h
    shader.h
    surface.h
    swapchain.h)

set(SOURCES
    command_buffers.cpp
    command_pool.cpp
    debug_messenger.cpp
    fence.cpp
    framebuffer.cpp
    graphics_pipeline.cpp
    helpers/file_helpers.cpp
    helpers/vulkan_collections_helpers.cpp
    helpers/vulkan_helpers.cpp
    image_view.cpp
    instance.cpp
    logical_device.cpp
    physical_device.cpp
    pipeline_layout.cpp
    pipeline.cpp
    queue_families_indices.cpp
    render_pass.cpp
    semaphore.cpp
    shader_module.cpp
    shader.cpp
    surface.cpp
    swapchain.cpp)

add_library(owlVulkan SHARED ${SOURCES} ${HEADERS})

target_include_directories(
  owlVulkan
  PRIVATE ${VULKAN_PATH}/Include
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/helpers)

target_link_libraries(owlVulkan PRIVATE ${VULKAN_PATH}/Lib/vulkan-1.lib)
