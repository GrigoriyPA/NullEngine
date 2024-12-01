set(TARGET_NAME null-engine-util-interface)

add_library(${TARGET_NAME}
    interface_holder.cpp
)

target_link_libraries(${TARGET_NAME}
    sfml-graphics
    sfml-system

    null-engine-util-geometry

    null-engine-util-interface-helpers
    null-engine-util-interface-objects
)

add_subdirectory(helpers)
add_subdirectory(objects)
