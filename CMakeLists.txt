cmake_minimum_required(VERSION 3.30)

option(USE_HOMEBREW_LLVM "Use Homebrew LLVM on macOS for C++23 modules" ON)

if(APPLE AND USE_HOMEBREW_LLVM)
    set(LLVM_PATH "/opt/homebrew/opt/llvm@20")  # Adjust if version changes

    set(CMAKE_C_COMPILER "${LLVM_PATH}/bin/clang")
    set(CMAKE_CXX_COMPILER "${LLVM_PATH}/bin/clang++")

    add_compile_options("-stdlib=libc++")
    add_link_options("-stdlib=libc++" "-L${LLVM_PATH}/lib")

    # Tell Clang where to find macOS system headers
    add_compile_options("-isysroot" "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
    add_link_options("-isysroot" "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD 23)

project(VKING)

# === Early compiler info message (appears near the top of configure output) ===
message(STATUS "Using C compiler:   ${CMAKE_C_COMPILER} (ID: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION})")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER} (ID: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})")

if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(WARNING "Apple Clang detected. C++23 modules may not work. "
            "Consider: brew install llvm && cmake -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++")
endif()

## Supported module Options
set(GLFW_BUILD_STATIC ON)
set(CMAKE_CXX_MODULE_STD OFF)

## Add supported modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake_Modules")
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)
find_package(spdlog REQUIRED)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Vulkan Found ${Vulkan_VERSION}")
else()
    message(FATAL_ERROR "Vulkan not found")
endif()

add_subdirectory(src)