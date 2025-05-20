set(TARGET_NAME null-engine-renderer-shaders)

add_library(${TARGET_NAME}
    multithread_fragment_shader.cpp
    native_fragment_shader.cpp
    vertex_shader.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    fmt::fmt
    null-engine-acceleration
    null-engine-drawable-objects-common
    null-engine-drawable-objects-material
    null-engine-scene-lights
    null-engine-util-geometry
    OpenCL::OpenCL
)
