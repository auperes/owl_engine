cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(OwlEngine
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(CMAKE_VERSION VERSION_LESS 3.7.0)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

file(TO_CMAKE_PATH $ENV{VK_SDK_PATH} VULKAN_PATH)
set(GLFW_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_parties/glfw-3.3.2.bin.WIN64/include)
set(GLFW_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../third_parties/glfw-3.3.2.bin.WIN64/lib-mingw-w64/libglfw3.a)
set(GLM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_parties/glm-0.9.9.8)
set(STB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_parties/stb)

add_subdirectory(owlVulkan)
add_subdirectory(owlGui)

set(HEADERS engine.h)

set(SOURCES main.cpp engine.cpp)

add_executable(OwlEngine ${SOURCES} ${HEADERS})

target_include_directories(
  OwlEngine
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}
  PRIVATE ${VULKAN_PATH}/Include
  PRIVATE ${GLFW_INCLUDE}
  PRIVATE ${GLM_INCLUDE}
  PRIVATE ${STB_INCLUDE}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/owlVulkan)

add_dependencies(OwlEngine owlVulkan)
target_link_libraries(
  OwlEngine
  PRIVATE ${GLFW_LIB}
  PRIVATE ${VULKAN_PATH}/Lib/vulkan-1.lib
          ${CMAKE_CURRENT_BINARY_DIR}/owlVulkan/libowlVulkan.dll.a)

#copy libowlVulkan.dll to .exe directory
add_custom_command(
  TARGET OwlEngine
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/owlVulkan/libowlVulkan.dll
    ${CMAKE_CURRENT_BINARY_DIR})

#copy resources in binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)
add_custom_command(
  TARGET OwlEngine
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures
    ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

#compile and coyp shaders in binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_custom_command(
  TARGET OwlEngine
  POST_BUILD
  COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/passthrough.vert
          -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/passthrough_vert.spv)

add_custom_command(
  TARGET OwlEngine
  POST_BUILD
  COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/passthrough.frag
          -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/passthrough_frag.spv)
