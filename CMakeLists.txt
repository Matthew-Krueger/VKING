cmake_minimum_required(VERSION 4.0)
project(VKING)

set(CMAKE_CXX_STANDARD 23)
set(BUILD_WITH_OPENMP ON)

## Supported module Options
set(GLFW_BUILD_STATIC ON)

## Add supported modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake_Modules")
include(BuildWithOpenMP)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)



add_executable(VKING main.cpp )
