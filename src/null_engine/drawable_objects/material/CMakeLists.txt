set(TARGET_NAME null-engine-drawable-objects-material)

add_library(${TARGET_NAME}
    texture.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
    sfml-graphics
)
