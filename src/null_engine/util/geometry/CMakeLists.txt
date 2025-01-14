set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    helpers.cpp
    matrix4.cpp
    vector2.cpp
    vector3.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
)
