set(TARGET_NAME null-engine-drawable-objects)

add_library(${TARGET_NAME}
    vertices_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-generic-mesh
    null-engine-util-generic
    null-engine-util-geometry
)
