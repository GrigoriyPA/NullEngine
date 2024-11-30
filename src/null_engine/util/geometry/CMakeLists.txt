set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    constants.cpp
    transformation.cpp
    vector_3d.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
)
