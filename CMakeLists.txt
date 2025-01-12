cmake_minimum_required(VERSION 3.24)

set(PROJECT_NAME null-engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/contrib/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(${PROJECT_NAME})

find_package(folly REQUIRED)

add_library(${PROJECT_NAME} STATIC
    src/null_engine/null_engine.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}
    null-engine-drawable-objects
    null-engine-generic
    null-engine-native
)

enable_testing()

add_subdirectory(contrib)
add_subdirectory(src/null_engine)
add_subdirectory(examples)
