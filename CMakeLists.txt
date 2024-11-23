cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME NullEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

project(${PROJECT_NAME})

enable_testing()

add_subdirectory(examples)
