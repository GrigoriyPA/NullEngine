set(TARGET_NAME null-engine-generic-renderer)

add_library(${TARGET_NAME}
    generic_consumer_interface.cpp
    generic_renderer_interface.cpp
    generic_texture_consumer.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-generic-camera
    null-engine-generic-mesh
    null-engine-util-generic
    sfml-graphics
)
