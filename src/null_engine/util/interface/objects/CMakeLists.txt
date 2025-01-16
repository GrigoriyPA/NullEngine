set(TARGET_NAME null-engine-util-interface-objects)

add_library(${TARGET_NAME}
    fps_display.cpp
    texture_rendering_consumer.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-util-generic
    null-engine-util-geometry
    null-engine-util-mvc
    sfml-graphics
)
