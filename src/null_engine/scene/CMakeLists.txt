set(TARGET_NAME null-engine-scene)

add_library(${TARGET_NAME}
    scene.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects
)