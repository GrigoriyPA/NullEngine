set(TARGET_NAME null-engine-testlib)

add_library(${TARGET_NAME}
    testlib_base_application.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-window

    null-engine-generic

    null-engine-util-interface
)
