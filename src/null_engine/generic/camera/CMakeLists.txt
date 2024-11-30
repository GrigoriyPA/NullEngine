set(TARGET_NAME null-engine-generic-camera)

add_library(${TARGET_NAME}
    generic_camera_base.cpp
    generic_camera.cpp
    generic_direct_camera.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
    null-engine-util-geometry
)
