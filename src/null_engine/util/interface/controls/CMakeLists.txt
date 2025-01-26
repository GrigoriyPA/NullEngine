set(TARGET_NAME null-engine-util-interface-controls)

add_library(${TARGET_NAME}
    centering_mouse_control.cpp
    keyboard_control.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
    null-engine-util-mvc
    sfml-graphics
    sfml-window
)
