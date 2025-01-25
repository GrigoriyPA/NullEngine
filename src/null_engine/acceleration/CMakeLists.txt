set(TARGET_NAME null-engine-acceleration)

add_library(${TARGET_NAME}
    acceleration_context.cpp
)

target_link_libraries(${TARGET_NAME}
    Boost::compute
    fmt::fmt
    OpenCL::OpenCL
    sfml-graphics
)
