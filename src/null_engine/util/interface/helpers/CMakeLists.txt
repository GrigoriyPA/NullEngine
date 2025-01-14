set(TARGET_NAME null-engine-util-interface-helpers)

add_library(${TARGET_NAME}
    fonts.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics
)
