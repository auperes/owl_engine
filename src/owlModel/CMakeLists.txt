set(HEADERS
    mesh.h
    texture.h
    vertex.h)

set(SOURCES)

add_library(owlModel INTERFACE)

target_include_directories(
    owlModel
    INTERFACE ${GLM_INCLUDE}
    INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# target_link_libraries(owlModel)