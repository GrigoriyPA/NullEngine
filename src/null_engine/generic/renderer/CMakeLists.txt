set(TARGET_NAME null-engine-generic-renderer)

add_library(${TARGET_NAME}
    generic_renderer.cpp
    generic_rendering_consumer.cpp
    generic_texture_consumer.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics

    null-engine-generic-camera
    null-engine-generic-mesh

    null-engine-util-generic
)
