set(HEADERS
    buffer.h
    command_buffers.h
    command_pool.h
    debug_messenger.h
    descriptor_pool.h
    descriptor_set_layout.h
    descriptor_sets.h
    device_memory.h
    fence.h
    framebuffer.h
    graphics_pipeline.h
    helpers/file_helpers.h
    helpers/vulkan_collections_helpers.h
    helpers/vulkan_helpers.h
    image_view.h
    image.h
    instance.h
    logical_device.h
    matrix.h
    physical_device.h
    pipeline_layout.h
    pipeline.h
    queue_families_indices.h
    render_pass.h
    sampler.h
    semaphore.h
    shader_module.h
    surface.h
    swapchain.h
    swapchain_support.h
    vertex.h
    vulkan_object.h)

set(SOURCES
    buffer.cpp
    command_buffers.cpp
    command_pool.cpp
    debug_messenger.cpp
    descriptor_pool.cpp
    descriptor_set_layout.cpp
    descriptor_sets.cpp
    device_memory.cpp
    fence.cpp
    framebuffer.cpp
    graphics_pipeline.cpp
    helpers/file_helpers.cpp
    helpers/vulkan_collections_helpers.cpp
    helpers/vulkan_helpers.cpp
    image_view.cpp
    image.cpp
    instance.cpp
    logical_device.cpp
    physical_device.cpp
    pipeline_layout.cpp
    pipeline.cpp
    queue_families_indices.cpp
    render_pass.cpp
    sampler.cpp
    semaphore.cpp
    shader_module.cpp
    surface.cpp
    swapchain.cpp
    swapchain_support.cpp
    vertex.cpp)

add_library(owlVulkan SHARED ${SOURCES} ${HEADERS})

target_include_directories(
  owlVulkan
  PRIVATE ${VULKAN_PATH}/Include
  PRIVATE ${GLM_INCLUDE}
  PRIVATE ${PROJECT_SOURCE_DIR}/owlModel
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/helpers)

add_dependencies(owlVulkan owlModel)
target_link_libraries(
  owlVulkan
  PRIVATE ${VULKAN_PATH}/Lib/vulkan-1.lib)
  # ${CMAKE_CURRENT_BINARY_DIR}/../owlModel/libowlModel.dll.a)
