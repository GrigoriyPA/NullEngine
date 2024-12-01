set(TARGET_NAME null-engine-util-interface-objects)

add_library(${TARGET_NAME}
    fps_counter.cpp
    interface_object.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics
    sfml-system
    sfml-window

    null-engine-util-generic
    null-engine-util-geometry
)
