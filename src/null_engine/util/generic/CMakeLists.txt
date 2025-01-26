set(TARGET_NAME null-engine-util-generic)

add_library(${TARGET_NAME}
    validation.cpp
)

target_link_libraries(${TARGET_NAME}
    fmt::fmt
)
