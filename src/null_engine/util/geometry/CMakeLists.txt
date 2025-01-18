set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    helpers.cpp
    line2.cpp
    matrix3.cpp
    matrix4.cpp
    vector2.cpp
    vector3.cpp
    vector4.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
)
