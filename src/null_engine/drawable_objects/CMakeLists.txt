set(TARGET_NAME null-engine-drawable-objects)

add_library(${TARGET_NAME}
    vertex.cpp
    vertices_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
)
