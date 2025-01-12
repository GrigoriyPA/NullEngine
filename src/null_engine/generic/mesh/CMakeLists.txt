set(TARGET_NAME null-engine-generic-mesh)

add_library(${TARGET_NAME}
    generic_mesh_interface.cpp
    generic_vertex.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
)
