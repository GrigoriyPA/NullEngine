set(TARGET_NAME null-engine-drawable-objects)

add_library(${TARGET_NAME}
    primitive_objects.cpp
    vertices_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
    null-engine-drawable-objects-material
    null-engine-util-geometry
)

add_subdirectory(common)
add_subdirectory(material)
