set(TARGET_NAME null-engine-util-interface-objects)

add_library(${TARGET_NAME}
    fps_counter.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-util-generic
    null-engine-util-geometry
    sfml-graphics
    sfml-system
    sfml-window
)
