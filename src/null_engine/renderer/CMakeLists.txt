set(TARGET_NAME null-engine-renderer)

add_library(${TARGET_NAME}
    renderer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-renderer-camera
    null-engine-renderer-consumers
    null-engine-renderer-rasterization
    null-engine-scene
    null-engine-util-geometry
    null-engine-util-mvc
)

add_subdirectory(camera)
add_subdirectory(consumers)
add_subdirectory(rasterization)
