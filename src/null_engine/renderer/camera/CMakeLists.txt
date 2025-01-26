set(TARGET_NAME null-engine-renderer-camera)

add_library(${TARGET_NAME}
    camera.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
    null-engine-util-mvc
)
