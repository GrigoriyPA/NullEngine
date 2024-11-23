set(TARGET_NAME quick_start_native_example)

add_executable(${TARGET_NAME}
    quick_start_native.cpp
)

add_test(test_${TARGET_NAME} ${TARGET_NAME})
