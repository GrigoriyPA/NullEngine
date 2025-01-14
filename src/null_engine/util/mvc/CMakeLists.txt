set(TARGET_NAME null-engine-util-mvc)

add_library(${TARGET_NAME}
    ports.hpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-generic
)
