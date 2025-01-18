set(TARGET_NAME null-engine-renderer-shaders)

add_library(${TARGET_NAME}
    vertex_shader.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
    null-engine-util-geometry
)
