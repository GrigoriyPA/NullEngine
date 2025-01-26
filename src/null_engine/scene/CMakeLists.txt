set(TARGET_NAME null-engine-scene)

add_library(${TARGET_NAME}
    scene.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-scene-animations
    null-engine-scene-lights
    null-engine-scene-objects
)

add_subdirectory(animations)
add_subdirectory(lights)
add_subdirectory(objects)
