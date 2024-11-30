set(TARGET_NAME null-engine-native-common)

add_library(${TARGET_NAME}
    native_vertex.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
)
