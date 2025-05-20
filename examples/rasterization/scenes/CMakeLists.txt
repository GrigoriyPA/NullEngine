set(TARGET_NAME null-engine-example-rasterization-scenes)

add_library(${TARGET_NAME}
    example_scenes.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine
    null-engine-tests
)
