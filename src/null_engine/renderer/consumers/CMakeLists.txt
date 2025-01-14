set(TARGET_NAME null-engine-renderer-consumers)

add_library(${TARGET_NAME}
    texture_rendering_consumer.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-renderer-rasterization
    null-engine-util-generic
    null-engine-util-mvc
    sfml-graphics
)
