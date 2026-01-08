# =============================================================================
# VKING Editor Executable
# =============================================================================
# This target builds the VKING Editor application, which serves as both a
# development tool and demonstration of the engine's capabilities.
# =============================================================================

# Define the editor executable and its main entry point source file
add_executable(VKING_Editor main.cpp)

# =============================================================================
# C++23 Modules
# =============================================================================
# The editor defines its own module interface for editor-specific functionality.
# For executables, FILE_SET visibility (PUBLIC/PRIVATE) doesn't affect dependents
# since executables aren't linked against by other targets. We use PUBLIC here
# to match CMake conventions for module file sets.
# =============================================================================
target_sources(VKING_Editor
        PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES
        FILES
        # Editor-specific application module
        # This defines the editor's Application class and entry point logic
        Application.ixx
)

# =============================================================================
# Link Dependencies
# =============================================================================
# The editor links against the VKING Engine to access all core functionality.
# PRIVATE is correct because the editor doesn't expose engine APIs to any
# dependents (executables don't have dependents).
# =============================================================================
target_link_libraries(VKING_Editor PRIVATE VKING::Engine)

# =============================================================================
# Precompiled Headers
# =============================================================================
# Reuse the engine's precompiled headers to dramatically reduce compilation time.
# This shares the PCH binary generated for VKING_Engine, avoiding redundant
# parsing of GLM, STL headers, etc. (~15 seconds → ~2 seconds per TU).
# =============================================================================
target_precompile_headers(VKING_Editor REUSE_FROM VKING::Engine)

# =============================================================================
# Compiler Warnings
# =============================================================================
# Apply project-wide warning settings to ensure code quality and consistency.
# Uses the warning configuration defined in VKING_Warnings.cmake.
# =============================================================================
vking_apply_warnings(VKING_Editor)

# =============================================================================
# Build Dependency
# =============================================================================
# Ensure plugin is built before editor, but don't link it
# The engine loads it at runtime via dlopen/LoadLibrary
# =============================================================================

add_dependencies(VKING_Editor VKING_Render_GLFW_Vulkan)

# In Projects/Editor/CMakeLists.txt
add_custom_command(TARGET VKING_Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:VKING_Editor>/../plugins/Renderer/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:VKING_Render_GLFW_Vulkan>
        $<TARGET_FILE_DIR:VKING_Editor>/../plugins/Renderer/
        # DEPENDS removed - not supported here
)

# Optional: copy plugin to editor's output directory for convenience
add_custom_command(TARGET VKING_Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:VKING_Render_GLFW_Vulkan>
        $<TARGET_FILE_DIR:VKING_Editor>/../plugins/Renderer/
)

# =============================================================================
# Namespaced Alias
# =============================================================================
# Creates VKING::Editor as an alias for VKING_Editor.
# This follows modern CMake best practices, allowing other parts of the build
# system to refer to the editor target using a consistent, namespaced identifier.
# Note: add_library is used for aliases, even for executable targets.
# =============================================================================
add_executable(VKING::Editor ALIAS VKING_Editor)