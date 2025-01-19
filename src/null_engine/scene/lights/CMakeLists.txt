set(TARGET_NAME null-engine-scene-lights)

add_library(${TARGET_NAME}
    light.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
    null-engine-util-geometry
)
