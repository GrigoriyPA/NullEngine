cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME null_engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wimplicit-fallthrough -g")

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
    include/null_engine/null_engine.hpp
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(${PROJECT_NAME}
    null_engine_implementation
)

enable_testing()

add_subdirectory(src/null_engine)
add_subdirectory(examples)
