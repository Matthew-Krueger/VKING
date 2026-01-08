# =============================================================================
# VKING Engine - Root CMake Configuration
# =============================================================================
# This is the root CMakeLists.txt for the VKING game engine project.
# It configures the build environment, compiler, dependencies, and orchestrates
# the build of all engine components, plugins, and tools.
#
# Build Requirements:
#   - CMake 3.30+ (for C++23 module support)
#   - C++23 compatible compiler (Clang 20+ recommended on macOS)
#   - Vulkan SDK, GLFW, GLM, spdlog, fmt
#
# macOS Homebrew LLVM Setup:
#   By default, this configuration uses Homebrew LLVM for C++23 module support,
#   as Apple Clang's module support is incomplete. To override:
#     cmake -DUSE_HOMEBREW_LLVM=OFF ..
#   Or specify a different LLVM version:
#     cmake -DUSE_HOMEBREW_LLVM=ON -DLLVM_VERSION=21 ..
#
# =============================================================================

# =============================================================================
# CMake Minimum Version and Project Definition
# =============================================================================
# Must be first: sets CMake version requirements before any other commands.
# CMake 3.30+ is required for robust C++23 module support and FILE_SETS.
# =============================================================================
cmake_minimum_required(VERSION 3.30)

# =============================================================================
# Project-wide Options
# =============================================================================
# These options control build behavior and can be set from command line:
#   cmake -DOPTION_NAME=ON/OFF ..
# =============================================================================

# Use Homebrew LLVM on macOS for proper C++23 module support.
# Apple Clang's module implementation is incomplete and will cause build failures.
# When OFF, uses system default compiler (risky on macOS).
option(USE_HOMEBREW_LLVM "Use Homebrew LLVM on macOS for C++23 modules" ON)

# Enable Vulkan rendering backend support.
# Disable to build engine without Vulkan (useful for headless/server builds).
option(VKING_ENABLE_VULKAN "Enable Vulkan rendering backend support" ON)

# Enable GLFW windowing system support.
# Disable to build engine without windowing (useful for headless/server builds).
option(VKING_ENABLE_GLFW "Enable GLFW windowing system support" ON)

# Enable pedantic compiler warnings.
# Turn OFF for faster builds during rapid iteration (warnings still enabled, just less strict).
option(VKING_PEDANTIC_WARNINGS "Enable ultra-pedantic compiler warnings (may be noisy)" ON)

# =============================================================================
# Compiler Configuration (macOS Homebrew LLVM)
# =============================================================================
# On macOS, Apple Clang's C++23 module support is incomplete. We use Homebrew LLVM
# by default for reliable module compilation. This section:
#   1. Detects macOS + Homebrew LLVM request
#   2. Sets compiler paths to Homebrew LLVM
#   3. Configures libc++ and system SDK paths
#   4. Warns if using Apple Clang (which will likely fail)
# =============================================================================
if(APPLE AND USE_HOMEBREW_LLVM)
    # Allow overriding LLVM version from command line
    set(LLVM_VERSION "20" CACHE STRING "Homebrew LLVM version to use")

    # Construct Homebrew LLVM path (adjust if Homebrew is in non-standard location)
    set(LLVM_PATH "/opt/homebrew/opt/llvm@${LLVM_VERSION}")

    # Verify LLVM installation exists before proceeding
    if(NOT EXISTS "${LLVM_PATH}/bin/clang++")
        message(FATAL_ERROR "Homebrew LLVM ${LLVM_VERSION} not found at ${LLVM_PATH}. "
                "Install with: brew install llvm@${LLVM_VERSION}")
    endif()

    message(STATUS "Using Homebrew LLVM ${LLVM_VERSION} from ${LLVM_PATH}")

    # Set compiler to Homebrew LLVM
    set(CMAKE_C_COMPILER "${LLVM_PATH}/bin/clang" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${LLVM_PATH}/bin/clang++" CACHE FILEPATH "C++ compiler" FORCE)

    # Use libc++ from Homebrew LLVM (required for module support)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>")
    add_link_options("$<$<LINK_LANGUAGE:CXX>:-stdlib=libc++>" "-L${LLVM_PATH}/lib")

    # Tell Clang where to find macOS system headers
    # Without this, libc++ can't find system frameworks
    add_compile_options("-isysroot" "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
    add_link_options("-isysroot" "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
endif()

# =============================================================================
# C++ Standard and Module Configuration
# =============================================================================
# Enable C++23 standard across all targets.
# Must be set before any targets are defined.
# =============================================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler extensions for portability

# Enable CMake's built-in C++20 module dependency scanning.
# This automatically discovers module dependencies between translation units.
# Required for proper module build ordering and dependency tracking.
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Disable automatic std module import.
# We don't use std::* modules yet; keeping this OFF avoids conflicts with
# traditional #include <...> and reduces compile complexity.
set(CMAKE_CXX_MODULE_STD OFF)

# =============================================================================
# Project Definition
# =============================================================================
# Defines the project name, version, and languages.
# Must come after compiler setup but before dependency detection.
# =============================================================================
project(VKING
        VERSION 0.1.0
        DESCRIPTION "A high-performance, module-first game engine"
        HOMEPAGE_URL "https://github.com/yourusername/VKING"
        LANGUAGES C CXX
)

# =============================================================================
# Global Output Directories (Development)
# =============================================================================
# During development, place all build artifacts in predictable locations
# so the engine can find plugins and resources without 'make install'.
#
# CMAKE_RUNTIME_OUTPUT_DIRECTORY: Executables → <build>/bin/
# CMAKE_LIBRARY_OUTPUT_DIRECTORY: Shared libs (plugins) → <build>/plugins/
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY: Static libs → <build>/lib/
#
# These are CACHE variables for easy override:
#   cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/tmp/vking/bin ..
# =============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executable output directory")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins CACHE PATH "Shared library (plugin) output directory")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Static library output directory")

# =============================================================================
# Compiler Verification (macOS)
# =============================================================================
# Display compiler information and warn if using Apple Clang.
# Apple Clang will likely fail with C++23 modules; this warning helps diagnose.
# =============================================================================
message(STATUS "Using C compiler:   ${CMAKE_C_COMPILER} (ID: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION})")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER} (ID: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})")

if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(WARNING
            "Apple Clang detected. C++23 modules may not work correctly.\n"
            "Recommended: brew install llvm@20 && "
            "cmake -DUSE_HOMEBREW_LLVM=ON .."
    )
endif()

# =============================================================================
# Build Type Configuration
# =============================================================================
# Set default build type to RelWithDebInfo if not specified.
# Provides good performance while keeping debug symbols for profiling.
# =============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# =============================================================================
# Dependency Configuration
# =============================================================================
# Configure third-party dependencies before finding them.
# These variables control how dependencies are built/linked.
# =============================================================================

# Force static linking of GLFW to avoid runtime dependency issues.
# Plugins will link against the same GLFW statically.
set(GLFW_BUILD_STATIC ON CACHE BOOL "Build GLFW as static library")

# =============================================================================
# Position Independent Code (PIC)
# =============================================================================
# Enable Position Independent Code generation for ALL targets.
# This is REQUIRED for the plugin architecture to work correctly.
#
# Why PIC is mandatory:
#   - Render plugins are shared libraries (.dylib/.so/.dll) and MUST be PIC
#   - The static VKING_Engine library will be linked into both:
#       * Executables (Editor, etc.)
#       * Shared plugins (GLFWVulkan.dylib, etc.)
#   - Without PIC, linking a static library into a shared library fails on most platforms
#   - PIC has minimal performance impact on modern CPUs (<1% in most cases)
#
# Setting this globally ensures:
#   - All engine code is PIC, making it safe to link into plugins
#   - Plugin developers don't need to remember to enable it
#   - Consistent ABI between engine and plugins
#
# If you absolutely need non-PIC code for a specific target, override with:
#   set_target_properties(ThatTarget PROPERTIES POSITION_INDEPENDENT_CODE OFF)
# =============================================================================
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Enable Position Independent Code for all targets")

# =============================================================================
# CMake Module Path
# =============================================================================
# Add our custom CMake modules (Find*.cmake files) to CMake's search path.
# Must be done before any find_package() calls.
# =============================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake_Modules")

# =============================================================================
# Compiler Warnings Configuration
# =============================================================================
# Include our custom warning settings.
# Applies to all targets created after this point.
# =============================================================================
include(VKING_Warnings)

# =============================================================================
# Third-Party Dependencies
# =============================================================================
# Find all required external libraries.
# Each find_package() call should have a REQUIRED flag to fail fast if missing.
# =============================================================================

# GLFW: Windowing system (cross-platform)
find_package(GLFW REQUIRED)

# GLM: Mathematics library (vectors, matrices, etc.)
find_package(GLM REQUIRED)

# spdlog: Logging backend (host-side only)
find_package(spdlog REQUIRED)

# fmt: String formatting library (used by both engine and spdlog)
find_package(FMT REQUIRED)

# Vulkan: Graphics API
# REQUIRED ensures build fails immediately if Vulkan SDK is not found
find_package(Vulkan REQUIRED)
message(STATUS "Vulkan Found: ${Vulkan_VERSION}")

# =============================================================================
# Subdirectory Inclusion
# =============================================================================
# Recursively build all engine components, plugins, and tools.
# Order matters: src/ must come first as it defines VKING::Engine target
# that subsequent directories depend on.
# =============================================================================

# Build engine core library (defines VKING::Engine)
add_subdirectory(Engine)

# Build platform implementations (plugins)
# These depend on VKING::Engine for SDK headers
add_subdirectory(Platforms)

# Build applications (Editor, etc.)
# These depend on VKING::Engine
add_subdirectory(Projects)

# =============================================================================
# Testing (Future)
# =============================================================================
# Uncomment when ready to add tests:
# enable_testing()
# add_subdirectory(tests)
# =============================================================================

# =============================================================================
# Installation/Packaging (Future)
# =============================================================================
# When ready to support `make install`:
# include(GNUInstallDirs)
# install(TARGETS VKING_Engine VKING_Editor
#     EXPORT VKINGTargets
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )
# install(EXPORT VKINGTargets
#     FILE VKINGTargets.cmake
#     NAMESPACE VKING::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VKING
# )
# =============================================================================

# =============================================================================
# Summary
# =============================================================================
# Build configuration complete. To build:
#   cmake -B build -G Ninja
#   cmake --build build -j$(nproc)
#
# To run:
#   ./build/Projects/Editor/VKING_Editor
#
# For verbose module builds:
#   cmake --build build --verbose
# =============================================================================