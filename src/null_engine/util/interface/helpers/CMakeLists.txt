set(TARGET_NAME null-engine-util-interface-helpers)

add_library(${TARGET_NAME}
    events.cpp
    fonts.cpp
    timer.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics
    null-engine-util-generic
)
