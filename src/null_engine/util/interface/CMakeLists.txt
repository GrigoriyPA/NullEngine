set(TARGET_NAME null-engine-util-interface)

add_library(${TARGET_NAME}
    gui_interface.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-interface-controls
    null-engine-util-interface-helpers
    null-engine-util-interface-objects
)

add_subdirectory(controls)
add_subdirectory(helpers)
add_subdirectory(objects)
