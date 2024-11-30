set(TARGET_NAME null-engine-drawable-objects)

add_library(${TARGET_NAME}
    drawable_objects.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-drawable-objects-common
)
