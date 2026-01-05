# VKING_Warnings.cmake
# Reusable warning configurations for the VKING engine
#
# Usage:
#   include(VKING_Warnings)
#   vking_set_default_warnings(target_name)
#   vking_set_pedantic_warnings(target_name)  # optional, controlled by option

# =============================================================================
# Options
# =============================================================================

option(VKING_PEDANTIC_WARNINGS
        "Enable ultra-pedantic compiler warnings across VKING targets (may be noisy)" OFF)

# =============================================================================
# Interface libraries
# =============================================================================

# Reasonable default warnings — applied to all VKING targets
add_library(vking_default_warnings INTERFACE)
target_compile_options(vking_default_warnings INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:
        /W3          # Solid baseline for MSVC
        >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        -Wall
        -Wextra
        >
)

# Ultra-pedantic warnings — opt-in via VKING_PEDANTIC_WARNINGS
add_library(vking_pedantic_warnings INTERFACE)
target_compile_options(vking_pedantic_warnings INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
        /wd4996      # Optional: suppress common deprecated warnings if too noisy
        >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        # -Werror     # Uncomment if you want warnings-as-errors in pedantic mode
        >
)

# =============================================================================
# Helper functions
# =============================================================================

function(vking_apply_warnings target)
    target_link_libraries(${target} PRIVATE vking_default_warnings)

    if(VKING_PEDANTIC_WARNINGS)
        target_link_libraries(${target} PRIVATE vking_pedantic_warnings)
        message(STATUS "Pedantic warnings enabled for target: ${target}")
    endif()
endfunction()