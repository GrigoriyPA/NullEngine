set(TARGET_NAME null-engine-drawable-objects-material)

add_library(${TARGET_NAME}
    texture.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    null-engine-acceleration
    null-engine-util-geometry
    null-engine-util-interface-helpers
    OpenCL::OpenCL
    sfml-graphics
)
