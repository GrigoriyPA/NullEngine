set(TARGET_NAME null-engine-util-interface)

add_library(${TARGET_NAME}
    constants.cpp
    fonts.cpp
    fps_counter.cpp
    interface_holder.cpp
    interface_object.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics
    sfml-system

    null-engine-util-generic
    null-engine-util-geometry
)
