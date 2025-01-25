set(TARGET_NAME null-engine-scene-lights)

add_library(${TARGET_NAME}
    light.cpp
    scene_light.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
    null-engine-scene-animations
    null-engine-util-geometry
    null-engine-util-interface-helpers
    null-engine-util-mvc
)
