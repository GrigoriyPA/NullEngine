set(TARGET_NAME null-engine-scene-objects)

add_library(${TARGET_NAME}
    object_loader.cpp
    scene_object.cpp
)

target_link_libraries(${TARGET_NAME}
    assimp::assimp
    fmt::fmt
    null-engine-drawable-objects
    null-engine-drawable-objects-material
    null-engine-scene-animations
    null-engine-util-generic
    null-engine-util-geometry
)
