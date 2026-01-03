# FindGLM.cmake
# Always fetches a specific GLM version from GitHub using FetchContent.
# No attempt to find installed versions — ensures exact tested version.
# Usage: find_package(GLM REQUIRED)
# Provides target: glm::glm (header-only)

include(FetchContent)

# Option to disable fetching if needed (default ON)
option(GLM_FETCH "Always fetch GLM from GitHub" ON)

function(FetchGLM)
    set(GLM_VERSION "1.0.3" CACHE STRING "GLM version/tag to fetch")  # Latest as of January 2026

    message(STATUS "Fetching GLM ${GLM_VERSION} from GitHub")

    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG        ${GLM_VERSION}
            GIT_SHALLOW    TRUE
    )

    set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
    set(GLM_INSTALL     OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glm)

    message(STATUS "GLM ${GLM_VERSION} fetched and available as glm::glm")
endfunction()

# Always fetch if enabled (no installed detection)
if(GLM_FETCH)
    FetchGLM()
    set(glm_FOUND TRUE)

    # Legacy variables for compatibility (rarely needed with modern CMake)
    set(GLM_INCLUDE_DIR "${glm_SOURCE_DIR}")
endif()

# Standard find_package handling
if(glm_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(GLM
            REQUIRED_VARS GLM_INCLUDE_DIR
            HANDLE_COMPONENTS)

    # Ensure modern target exists (fetched GLM already provides it, but safe guard)
    if(NOT TARGET glm::glm)
        add_library(glm::glm INTERFACE IMPORTED)
        target_include_directories(glm::glm INTERFACE ${GLM_INCLUDE_DIR})
    endif()
endif()

mark_as_advanced(GLM_FETCH GLM_VERSION)