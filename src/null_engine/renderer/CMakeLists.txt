set(TARGET_NAME null-engine-native)

add_library(${TARGET_NAME}
    native_renderer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-generic-camera
    null-engine-generic-renderer
    null-engine-native-rasterization
    null-engine-util-generic
    null-engine-util-geometry
)

add_subdirectory(rasterization)
