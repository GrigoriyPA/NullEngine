set(TARGET_NAME null-engine-renderer-camera)

add_library(${TARGET_NAME}
    camera.cpp
    generic_camera_control.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
    null-engine-util-geometry
    null-engine-util-interface
    sfml-graphics
    sfml-system
    sfml-window
)
