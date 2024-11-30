set(TARGET_NAME null-engine-drawable-objects-common)

add_library(${TARGET_NAME}
    vertices_object.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-generic-mesh
    null-engine-native-common
    null-engine-util-geometry
)
