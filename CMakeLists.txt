cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME NullEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wimplicit-fallthrough -g")

project(${PROJECT_NAME})

enable_testing()

add_subdirectory(examples)
