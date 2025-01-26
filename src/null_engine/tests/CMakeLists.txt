set(TARGET_NAME null-engine-tests)

add_library(${TARGET_NAME}
    tests_helpers.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
    null-engine-renderer-rasterization
    null-engine-util-geometry
)
