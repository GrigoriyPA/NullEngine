set(TARGET_NAME null-engine-example-quick-start-native)

add_executable(${TARGET_NAME}
    quick_start_native.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/contrib/SFML/include)

target_link_libraries(${TARGET_NAME}
    null-engine
    null-engine-tests
)

add_dependencies(${TARGET_NAME} null-engine-example-copy-assets)
