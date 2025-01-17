set(TARGET_NAME null-engine-drawable-objects)

add_library(${TARGET_NAME}
    vertices_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
    null-engine-util-geometry
)

add_subdirectory(common)
