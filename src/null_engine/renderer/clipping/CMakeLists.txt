set(TARGET_NAME null-engine-renderer-clipping)

add_library(${TARGET_NAME}
    clipping.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
    null-engine-renderer-shaders
    null-engine-util-generic
)
