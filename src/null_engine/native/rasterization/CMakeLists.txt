set(TARGET_NAME null-engine-native-rasterization)

add_library(${TARGET_NAME}
    native_rasterizer_context.cpp
    native_rasterizer.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-native-common
)
