set(TARGET_NAME null-engine-util-interface)

add_library(${TARGET_NAME}
    interface_holder.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
    null-engine-util-interface-helpers
    null-engine-util-interface-objects
    sfml-graphics
    sfml-system
)

add_subdirectory(helpers)
add_subdirectory(objects)
