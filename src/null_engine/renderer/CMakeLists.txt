set(TARGET_NAME null-engine-renderer)

add_library(${TARGET_NAME}
    renderer.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    null-engine-renderer-camera
    null-engine-renderer-clipping
    null-engine-renderer-rasterization
    null-engine-renderer-shaders
    null-engine-scene
    null-engine-util-geometry
    null-engine-util-mvc
    sfml-graphics
)

add_subdirectory(camera)
add_subdirectory(clipping)
add_subdirectory(rasterization)
add_subdirectory(shaders)
