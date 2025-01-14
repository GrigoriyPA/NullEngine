set(TARGET_NAME null-engine-example-rasterization)

add_executable(${TARGET_NAME}
    application.cpp
    controller.cpp
    model.cpp
    rasterization_example.cpp
    view.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/contrib/SFML/include)

target_link_libraries(${TARGET_NAME}
    null-engine
    null-engine-tests
    null-engine-util-interface
)

add_dependencies(${TARGET_NAME} null-engine-example-copy-assets)
