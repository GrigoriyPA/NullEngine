set(TARGET_NAME null-engine-generic-renderer)

add_library(${TARGET_NAME}
    generic_texture_consumer.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-generic-camera
    null-engine-util-generic
    sfml-graphics
)
