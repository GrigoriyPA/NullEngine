set(TARGET_NAME null-engine-generic)

add_library(${TARGET_NAME}
    generic_scene.cpp
    generic_scene_renderer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-generic-camera
    null-engine-generic-renderer
    null-engine-util-generic
)

add_subdirectory(camera)
add_subdirectory(renderer)
