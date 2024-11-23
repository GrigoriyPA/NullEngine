set(TARGET_NAME quick_start_native_example)

add_executable(${TARGET_NAME}
    quick_start_native.cpp
)

target_link_libraries(${TARGET_NAME}
    null_engine
)
