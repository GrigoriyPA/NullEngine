set(TARGET_NAME null-engine-util-interface)

add_library(${TARGET_NAME}
    fps_display.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-util-generic
    null-engine-util-geometry
    null-engine-util-interface-controls
    null-engine-util-interface-helpers
    null-engine-util-mvc
    sfml-graphics
)

add_subdirectory(controls)
add_subdirectory(helpers)
