set(TARGET_NAME null-engine-scene-objects)

add_library(${TARGET_NAME}
    scene_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
    null-engine-scene-animations
)
