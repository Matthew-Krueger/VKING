# FindGLFW.cmake
# Always fetches a specific GLFW version from GitHub using FetchContent.
# No attempt to find installed versions — ensures exact tested version.
# Usage: find_package(GLFW REQUIRED)
# Provides target: GLFW::GLFW (alias to glfw)

include(FetchContent)

# Option to disable fetching if needed (default ON)
option(GLFW_FETCH "Always fetch GLFW from GitHub" ON)

function(FetchGLFW)
    set(GLFW_VERSION "3.4" CACHE STRING "GLFW version/tag to fetch")

    message(STATUS "Fetching GLFW ${GLFW_VERSION} from GitHub")

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        ${GLFW_VERSION}
            GIT_SHALLOW    TRUE
    )

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)

    # Fetched GLFW provides target 'glfw' — create standard alias
    if(NOT TARGET GLFW::GLFW)
        add_library(GLFW::GLFW ALIAS glfw)
    endif()

    message(STATUS "GLFW ${GLFW_VERSION} fetched and available as GLFW::GLFW")
endfunction()

# Always fetch if enabled (no installed detection)
if(GLFW_FETCH)
    FetchGLFW()
    set(GLFW_FOUND TRUE)

    # Legacy variables for compatibility (rarely needed with modern CMake)
    set(GLFW_INCLUDE_DIR "${glfw_SOURCE_DIR}/include")
    set(GLFW_LIBRARIES glfw)
endif()

# Standard find_package handling
if(GLFW_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(GLFW
            REQUIRED_VARS GLFW_LIBRARIES GLFW_INCLUDE_DIR
            HANDLE_COMPONENTS)
endif()

mark_as_advanced(GLFW_FETCH GLFW_VERSION)