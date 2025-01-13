set(TARGET_NAME null-engine-generic-camera)

add_library(${TARGET_NAME}
    generic_camera_base.cpp
    generic_camera_control.cpp
    generic_camera_interface.cpp
    generic_direct_camera.cpp
    generic_perspective_camera.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
    null-engine-util-geometry
    null-engine-util-interface
    sfml-graphics
    sfml-system
    sfml-window
)
