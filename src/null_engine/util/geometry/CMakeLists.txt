set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    helpers.cpp
    transformation.cpp
    vector_2d.cpp
    vector_3d.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
)
