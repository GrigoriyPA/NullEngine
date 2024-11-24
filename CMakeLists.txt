cmake_minimum_required(VERSION 3.24)

set(PROJECT_NAME null-engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
    include/null_engine/null_engine.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
    null-engine-implementation
)

enable_testing()

add_subdirectory(contrib)
add_subdirectory(src/null_engine)
add_subdirectory(examples)
