# ==============================================================================
# VKING Engine Shared Resources – Core module and header library
# ==============================================================================
# This is an OBJECT library containing:
#   • Public C++23 modules (e.g., VKING.Engine.Math)
#   • Public engine-wide prerequisites header (used as optional PCH)
# Consumers (Editor, Game, Tools, etc.) will link to this to get:
#   • Ability to `import VKING.Engine.Math;`
#   • Access to common types/macros via #include <VKING/Prerequisites.hpp>
# ==============================================================================

add_library(VKING_Platform_Glue_GLFWVulkan STATIC
        Platform.Glue.GLFWVulkan.ixx
)


# Nice namespaced alias for use throughout the project
add_library(VKING::Platform::Glue::GLFWVulkan ALIAS VKING_Platform_Glue_GLFWVulkan)

# -----------------------------------------------------------------------------
# Public C++23 modules
# -----------------------------------------------------------------------------
# These are PUBLIC because consumers need to be able to write:
#     import VKING.Engine.Math;
# in their own translation units.
# -----------------------------------------------------------------------------
target_sources(VKING_Platform_Glue_GLFWVulkan
        PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES
        FILES
        Platform.Glue.GLFWVulkan.ixx
)

# -----------------------------------------------------------------------------
# Regular sources (implementation files, private headers, etc.)
# -----------------------------------------------------------------------------
# Any .cpp files that implement module partitions or internal helpers go here.
# Prerequisites.hpp is listed here only so it's visible to CMake for PCH purposes.
# -----------------------------------------------------------------------------
target_sources(VKING_Platform_Glue_GLFWVulkan
        PRIVATE
        # include/VKING/Prerequisites.hpp  # Intentionally NOT listed as source
        # → It's a header-only PCH, not compiled directly into the object lib
        # src/SomeInternalImpl.cpp
)

# -----------------------------------------------------------------------------
# Public headers (for #include <VKING/...>)
# -----------------------------------------------------------------------------
# Consumers need access to the include/ directory to use Prerequisites.hpp
# and any other public headers you add later.
# Use generator expressions so this only applies during build, not install.
# -----------------------------------------------------------------------------
target_include_directories(VKING_Platform_Glue_GLFWVulkan
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # $<INSTALL_INTERFACE:include>  # Uncomment if you ever install the engine
)

# -----------------------------------------------------------------------------
# Precompiled Header – Opt-in for consumers
# -----------------------------------------------------------------------------
# We declare Prerequisites.hpp as a PCH header on this target.
# Consumers can choose to use it via:
#     target_precompile_headers(VKING::SharedResources REUSE_FROM VKING::SharedResources)
# This reuses our precompiled version without forcing it.
# If a consumer has their own PCH, they can simply ignore this.
# -----------------------------------------------------------------------------
target_precompile_headers(VKING_Platform_Glue_GLFWVulkan
        REUSE_FROM
        VKING::SharedResources
)

# -----------------------------------------------------------------------------
# Compile features and dependencies
# -----------------------------------------------------------------------------
target_compile_features(VKING_Platform_Glue_GLFWVulkan
        PUBLIC
        cxx_std_23  # Consumers inherit C++23 requirement
)

target_link_libraries(VKING_Platform_Glue_GLFWVulkan
        PRIVATE
        # Internal dependency – not propagated to consumers
        glfw
        Vulkan::Vulkan
        PUBLIC
        VKING::SharedResources
        # Public dependencies go here (e.g., Vulkan::Vulkan if you expose it)
        # Vulkan::Vulkan
)

# ==============================================================================
# Usage example for a consumer (e.g., Editor or Game executable):
# ==============================================================================
# add_executable(VKING_Editor ...)
# target_link_libraries(VKING_Editor PRIVATE VKING::SharedResources)
#
# # Optional: Reuse the engine's PCH for faster builds
# target_precompile_headers(VKING_Editor REUSE_FROM VKING::SharedResources)
# ==============================================================================