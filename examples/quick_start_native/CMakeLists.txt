set(TARGET_NAME quick-start-native-example)

add_executable(${TARGET_NAME}
    quick_start_native.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/contrib/SFML/include)

target_link_libraries(${TARGET_NAME}
    null-engine
    sfml-graphics
)
