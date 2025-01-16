set(TARGET_NAME null-engine-renderer-rasterization)

add_library(${TARGET_NAME}
    interpolation.cpp
    rasterizer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
    null-engine-util-geometry
)
