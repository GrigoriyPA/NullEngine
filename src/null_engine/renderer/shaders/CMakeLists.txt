set(TARGET_NAME null-engine-renderer-shaders)

add_library(${TARGET_NAME}
    fragment_shader.cpp
    vertex_shader.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
    null-engine-drawable-objects-material
    null-engine-util-geometry
)
