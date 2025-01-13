set(TARGET_NAME null-engine-tests)

add_library(${TARGET_NAME}
    tests_constants.cpp
    tests_exceptions.cpp
    tests_helpers.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-generic
    null-engine-native
    null-engine-util-generic
    null-engine-util-geometry
)
