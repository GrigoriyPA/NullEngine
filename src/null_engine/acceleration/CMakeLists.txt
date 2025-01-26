set(TARGET_NAME null-engine-acceleration)

add_library(${TARGET_NAME}
    acceleration_context.cpp
    helpers.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    fmt::fmt
    null-engine-util-generic
    OpenCL::OpenCL
    sfml-graphics
)
