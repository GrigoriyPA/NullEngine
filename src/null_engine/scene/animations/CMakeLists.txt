set(TARGET_NAME null-engine-scene-animations)

add_library(${TARGET_NAME}
    animation.cpp
    animator.cpp
    primitive_animations.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
    null-engine-util-mvc
)
