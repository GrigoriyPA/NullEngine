add_custom_target(null-engine-example-copy-assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
)

add_subdirectory(quick_start_native)
