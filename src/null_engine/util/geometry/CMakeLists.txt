set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    constants.cpp
    helpers.cpp
    transformation.cpp
    vector_3d.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
    null-engine-util-generic
)
