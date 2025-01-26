set(TARGET_NAME null-engine-renderer-rasterization)

add_library(${TARGET_NAME}
    common.cpp
    interpolation.cpp
    multithread_rasterizer.cpp
    native_rasterizer.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    null-engine-acceleration
    null-engine-drawable-objects
    null-engine-renderer-shaders
    null-engine-util-generic
    null-engine-util-geometry
    sfml-graphics
)
