set(TARGET_NAME null-engine-drawable-objects-common)

add_library(${TARGET_NAME}
    vertex.cpp
)

target_link_libraries(${TARGET_NAME}
    null-engine-util-geometry
)
