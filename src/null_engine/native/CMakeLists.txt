set(TARGET_NAME null-engine-native)

add_library(${TARGET_NAME}
    native_renderer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-native-common
    null-engine-native-rasterization
)

add_subdirectory(common)
add_subdirectory(rasterization)
