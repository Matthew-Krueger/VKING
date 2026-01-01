# BuildWithOpenMP.cmake
# Configures OpenMP support if BUILD_WITH_OPENMP is ON.
# - Uses native find_package(OpenMP) where possible (CMake 3.12+ handles macOS brew libomp).
# - Falls back to manual config on Apple (requires brew install libomp).
# - Creates imported targets: OpenMP::OpenMP_C and OpenMP::OpenMP_CXX.
# - Usage: include(BuildWithOpenMP) in CMakeLists.txt.
# - Then link targets: target_link_libraries(my_target PRIVATE OpenMP::OpenMP_CXX)
# - Debug: Set CMAKE_OPENMP_DEBUG=ON for verbose output.

cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0077 NEW)

# Debug verbosity control
option(CMAKE_OPENMP_DEBUG "Enable verbose debug output for OpenMP configuration" OFF)

# Main option
option(BUILD_WITH_OPENMP "Enable OpenMP support" OFF)

if(NOT BUILD_WITH_OPENMP)
    if(CMAKE_OPENMP_DEBUG)
        message(STATUS "OpenMP: BUILD_WITH_OPENMP is OFF - skipping configuration.")
    endif()
    return()
endif()

if(CMAKE_OPENMP_DEBUG)
    message(STATUS "OpenMP: Starting configuration (debug mode enabled).")
endif()

# Clear any prior config
set(OpenMP_FOUND FALSE)
set(MANUAL_OMP_CONFIG FALSE)

# Native detection first (handles most cases, including macOS with brew libomp)
if(CMAKE_OPENMP_DEBUG)
    message(STATUS "OpenMP: Attempting native find_package(OpenMP)...")
endif()
find_package(OpenMP QUIET)

if(OpenMP_FOUND)
    if(CMAKE_OPENMP_DEBUG)
        message(STATUS "OpenMP: Native detection successful.")
        message(STATUS "OpenMP:   Components found - C: ${OpenMP_C_FOUND}, CXX: ${OpenMP_CXX_FOUND}")
        if(OpenMP_C_FOUND)
            message(STATUS "OpenMP:   C include dirs: ${OpenMP_C_INCLUDE_DIR}")
            message(STATUS "OpenMP:   C flags: ${OpenMP_C_FLAGS}")
            message(STATUS "OpenMP:   C libraries: ${OpenMP_C_LIB_NAMES}")
        endif()
        if(OpenMP_CXX_FOUND)
            message(STATUS "OpenMP:   CXX include dirs: ${OpenMP_CXX_INCLUDE_DIR}")
            message(STATUS "OpenMP:   CXX flags: ${OpenMP_CXX_FLAGS}")
            message(STATUS "OpenMP:   CXX libraries: ${OpenMP_CXX_LIB_NAMES}")
        endif()
    endif()

    # Verify headers exist for native detection
    set(OPENMP_HEADER_CHECKED FALSE)
    if(OpenMP_C_FOUND OR OpenMP_CXX_FOUND)
        set(OMP_INCLUDE "${OpenMP_C_INCLUDE_DIR}")
        if(NOT OMP_INCLUDE AND OpenMP_CXX_FOUND)
            set(OMP_INCLUDE "${OpenMP_CXX_INCLUDE_DIR}")
        endif()
        if(OMP_INCLUDE)
            set(OPENMP_HEADER "${OMP_INCLUDE}/omp.h")
            if(EXISTS "${OPENMP_HEADER}")
                if(CMAKE_OPENMP_DEBUG)
                    message(STATUS "OpenMP: Native omp.h verified at: ${OPENMP_HEADER}")
                endif()
                set(OPENMP_HEADER_CHECKED TRUE)
            else()
                message(WARNING "OpenMP: Native detection found library but omp.h missing at expected path: ${OPENMP_HEADER}")
            endif()
        endif()
    endif()

    if(NOT OPENMP_HEADER_CHECKED AND CMAKE_OPENMP_DEBUG)
        message(STATUS "OpenMP: Native header verification incomplete - proceeding with library-only config.")
    endif()

else()
    if(CMAKE_OPENMP_DEBUG)
        message(STATUS "OpenMP: Native detection failed, trying manual fallback...")
    endif()

    # Fallback: Manual config for Apple (adapt for other platforms if needed)
    if(APPLE)
        # Check for brew first
        execute_process(
                COMMAND brew --prefix libomp
                OUTPUT_VARIABLE LIBOMP_PREFIX
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
                RESULT_VARIABLE BREW_RESULT
        )

        if(BREW_RESULT EQUAL 0 AND LIBOMP_PREFIX)
            set(MANUAL_OMP_CONFIG TRUE)
            set(OMP_INCLUDE_DIR "${LIBOMP_PREFIX}/include")
            set(OMP_LIBRARY "${LIBOMP_PREFIX}/lib/libomp.dylib")

            if(CMAKE_OPENMP_DEBUG)
                message(STATUS "OpenMP: Manual config - libomp prefix: ${LIBOMP_PREFIX}")
                message(STATUS "OpenMP:   Include dir: ${OMP_INCLUDE_DIR}")
                message(STATUS "OpenMP:   Library: ${OMP_LIBRARY}")
            endif()

            # Verify paths exist
            if(NOT EXISTS "${OMP_INCLUDE_DIR}")
                message(FATAL_ERROR "OpenMP: Manual include directory does not exist: ${OMP_INCLUDE_DIR}")
            endif()
            if(NOT EXISTS "${OMP_LIBRARY}")
                message(FATAL_ERROR "OpenMP: Manual library file does not exist: ${OMP_LIBRARY}")
            endif()

            # Check for omp.h specifically
            set(OMP_HEADER "${OMP_INCLUDE_DIR}/omp.h")
            if(EXISTS "${OMP_HEADER}")
                if(CMAKE_OPENMP_DEBUG)
                    message(STATUS "OpenMP: Manual omp.h verified at: ${OMP_HEADER}")
                endif()
            else()
                message(WARNING "OpenMP: Manual config found libomp but omp.h missing: ${OMP_HEADER}. Compilation may fail.")
            endif()

            # Flags for Clang (common on macOS); adjust if using GCC
            set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${OMP_INCLUDE_DIR}")
            set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${OMP_INCLUDE_DIR}")

            # Set variables for compatibility with CMake's OpenMP module
            set(OpenMP_C_FOUND TRUE)
            set(OpenMP_CXX_FOUND TRUE)
            set(OpenMP_C_INCLUDE_DIR "${OMP_INCLUDE_DIR}")
            set(OpenMP_CXX_INCLUDE_DIR "${OMP_INCLUDE_DIR}")
            set(OpenMP_C_LIB_NAMES "omp")
            set(OpenMP_CXX_LIB_NAMES "omp")
            set(OpenMP_omp_LIBRARY "${OMP_LIBRARY}")
            set(OpenMP_FOUND TRUE)

            if(CMAKE_OPENMP_DEBUG)
                message(STATUS "OpenMP: Manual config - C flags: ${OpenMP_C_FLAGS}")
                message(STATUS "OpenMP: Manual config - CXX flags: ${OpenMP_CXX_FLAGS}")
            endif()
        else()
            message(WARNING "OpenMP: libomp not found via brew. Install with 'brew install libomp' or disable BUILD_WITH_OPENMP.")
            if(CMAKE_OPENMP_DEBUG)
                message(STATUS "OpenMP: Brew result code: ${BREW_RESULT}, prefix: ${LIBOMP_PREFIX}")
            endif()
        endif()
    else()
        message(WARNING "OpenMP: Manual fallback only implemented for Apple. For other platforms, install libomp via package manager.")
    endif()

    # If still not found after manual attempt
    if(NOT OpenMP_FOUND)
        message(WARNING "OpenMP: No configuration method succeeded. Compiling without OpenMP support.")
        if(BUILD_WITH_OPENMP)
            message(FATAL_ERROR "BUILD_WITH_OPENMP is ON but all OpenMP detection methods failed.")
        endif()
        return()
    endif()
endif()

# Create imported targets for modern usage (CMake 3.12+ style)
# These handle include dirs, flags, and linking automatically.

if(OpenMP_C_FOUND)
    if(NOT TARGET OpenMP::OpenMP_C)
        add_library(OpenMP::OpenMP_C INTERFACE IMPORTED)
        set_target_properties(OpenMP::OpenMP_C PROPERTIES
                INTERFACE_COMPILE_OPTIONS "${OpenMP_C_FLAGS}"
                INTERFACE_LINK_LIBRARIES "${OpenMP_omp_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${OpenMP_C_INCLUDE_DIR}"
        )
        if(CMAKE_OPENMP_DEBUG)
            message(STATUS "OpenMP: Created C target with include: ${OpenMP_C_INCLUDE_DIR}")
        endif()
    else()
        if(CMAKE_OPENMP_DEBUG)
            message(STATUS "OpenMP: C target already exists - skipping creation.")
        endif()
    endif()
endif()

if(OpenMP_CXX_FOUND)
    if(NOT TARGET OpenMP::OpenMP_CXX)
        add_library(OpenMP::OpenMP_CXX INTERFACE IMPORTED)
        set_target_properties(OpenMP::OpenMP_CXX PROPERTIES
                INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
                INTERFACE_LINK_LIBRARIES "${OpenMP_omp_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${OpenMP_CXX_INCLUDE_DIR}"
        )
        if(CMAKE_OPENMP_DEBUG)
            message(STATUS "OpenMP: Created CXX target with include: ${OpenMP_CXX_INCLUDE_DIR}")
        endif()
    else()
        if(CMAKE_OPENMP_DEBUG)
            message(STATUS "OpenMP: CXX target already exists - skipping creation.")
        endif()
    endif()
endif()

# Final summary
if(OpenMP_FOUND)
    message(STATUS "OpenMP: Configuration complete. Available targets:")
    if(OpenMP_C_FOUND)
        message(STATUS "OpenMP:   - OpenMP::OpenMP_C (headers: ${OpenMP_C_INCLUDE_DIR})")
    endif()
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP:   - OpenMP::OpenMP_CXX (headers: ${OpenMP_CXX_INCLUDE_DIR})")
    endif()
    message(STATUS "OpenMP:   Link your targets with these for automatic flag/include propagation.")

    if(CMAKE_OPENMP_DEBUG)
        message(STATUS "OpenMP: Debug summary - Manual config used: ${MANUAL_OMP_CONFIG}")
        message(STATUS "OpenMP: Debug summary - Final library path: ${OpenMP_omp_LIBRARY}")
    endif()

    # Set a cache variable for projects to check
    set(OpenMP_CONFIGURED TRUE CACHE INTERNAL "OpenMP configuration status")
else()
    set(OpenMP_CONFIGURED FALSE CACHE INTERNAL "OpenMP configuration status")
endif()

if(CMAKE_OPENMP_DEBUG)
    message(STATUS "OpenMP: Configuration process complete.")
endif()