# =============================================================================
# Plugin Build Configuration with Dependency Graph
# =============================================================================
# This system ensures shared dependencies (GLFW, Vulkan, etc.) are built ONCE
# and BEFORE any plugins that need them. It also prevents redundant builds
# when multiple plugins share the same dependency.
#
# Dependency Graph Structure:
#   Windowing Backends (static libs):
#     - GLFW, Wayland, X11, Cocoa, Win32
#
#   Rendering Backends (static libs):
#     - Vulkan, Metal, OpenGL, DirectX12, GNM
#
#   Glue Plugins (shared libs):
#     - Combine ONE windowing + ONE rendering backend
#     - Example: GLFWVulkan, WaylandVulkan, CocoaMetal
#
# Build Order (MANDATORY):
#   1. Windowing backends (if any plugin needs them)
#   2. Rendering backends (if any plugin needs them)
#   3. Glue plugins (depends on both)
# =============================================================================

# =============================================================================
# Plugin Build Options
# =============================================================================
# Each option controls whether a specific GLUE plugin is built.
# Add new plugins here as they are implemented.
# =============================================================================
option(VKING_BUILD_PLUGIN_GLFW_VULKAN "Build GLFW+Vulkan render plugin" ON)
option(VKING_BUILD_PLUGIN_WAYLAND_VULKAN "Build Wayland+Vulkan render plugin" OFF)
option(VKING_BUILD_PLUGIN_COCOA_METAL "Build Cocoa+Metal render plugin" OFF)
# Add more as needed: VKING_BUILD_PLUGIN_X11_VULKAN, etc.

# =============================================================================
# Dependency Tracking Variables
# =============================================================================
# These variables accumulate which backends are needed by ANY enabled plugin.
# They start OFF and get flipped to ON if a plugin requires them.
#
# Example: If both GLFW_VULKAN and GLFW_METAL are enabled,
# NEED_GLFW becomes ON (but GLFW is only built once).
# =============================================================================
set(NEED_GLFW OFF)
set(NEED_WAYLAND OFF)
set(NEED_X11 OFF)
set(NEED_COCOA OFF)
set(NEED_WIN32 OFF)

set(NEED_VULKAN OFF)
set(NEED_METAL OFF)
set(NEED_OPENGL OFF)
set(NEED_DIRECTX_12 OFF)
set(NEED_GNM OFF)

# =============================================================================
# Analyze Plugin Dependencies
# =============================================================================
# For each enabled plugin, mark its dependencies as needed.
# This is the "dependency resolution" step.
# =============================================================================

if(VKING_BUILD_PLUGIN_GLFW_VULKAN)
    set(NEED_GLFW ON)
    set(NEED_VULKAN ON)
endif()

if(VKING_BUILD_PLUGIN_WAYLAND_VULKAN)
    set(NEED_WAYLAND ON)
    set(NEED_VULKAN ON)
endif()

if(VKING_BUILD_PLUGIN_COCOA_METAL)
    set(NEED_COCOA ON)
    set(NEED_METAL ON)
endif()

# Add new plugin dependency analysis here:
# if(VKING_BUILD_PLUGIN_X11_VULKAN)
#     set(NEED_X11 ON)
#     set(NEED_VULKAN ON)
# endif()

# =============================================================================
# Build Dependencies (Windowing Backends)
# =============================================================================
# Build each windowing backend if ANY plugin needs it.
# The if() conditions ensure each add_subdirectory() is called AT MOST ONCE.
# =============================================================================

if(NEED_GLFW)
    add_subdirectory(Windowing/GLFW)
endif()

if(NEED_WAYLAND)
    add_subdirectory(Windowing/Wayland)
endif()

if(NEED_X11)
    add_subdirectory(Windowing/X11)
endif()

if(NEED_COCOA)
    add_subdirectory(Windowing/Cocoa)
endif()

if(NEED_WIN32)
    add_subdirectory(Windowing/Win32)
endif()

# =============================================================================
# Build Dependencies (Rendering Backends)
# =============================================================================
# Build each rendering backend if ANY plugin needs it.
# Again, each add_subdirectory() is called AT MOST ONCE.
# =============================================================================

if(NEED_VULKAN)
    add_subdirectory(RenderingBackends/Vulkan)
endif()

if(NEED_METAL)
    add_subdirectory(RenderingBackends/Metal)
endif()

if(NEED_OPENGL)
    add_subdirectory(RenderingBackends/OpenGL)
endif()

if(NEED_DIRECTX_12)
    add_subdirectory(RenderingBackends/DirectX12)
endif()

if(NEED_GNM)
    add_subdirectory(RenderingBackends/GNM)
endif()

# =============================================================================
# Build Glue Plugins
# =============================================================================
# Now that all dependencies are built, we can safely build the plugins.
# Each plugin links against its specific dependencies (which are now available).
# =============================================================================

if(VKING_BUILD_PLUGIN_GLFW_VULKAN)
    add_subdirectory(Glue/GLFWVulkan)
endif()

if(VKING_BUILD_PLUGIN_WAYLAND_VULKAN)
    add_subdirectory(Glue/WaylandVulkan)
endif()

if(VKING_BUILD_PLUGIN_COCOA_METAL)
    add_subdirectory(Glue/CocoaMetal)
endif()

# Add new plugin builds here:
# if(VKING_BUILD_PLUGIN_X11_VULKAN)
#     add_subdirectory(Glue/X11Vulkan)
# endif()

# =============================================================================
# Extending the System (How to Add a New Plugin)
# =============================================================================
# To add a new plugin (e.g., X11 + OpenGL):
#
# 1. Create directories:
#    Platforms/Windowing/X11/
#    Platforms/RenderingBackends/OpenGL/
#    Platforms/Glue/X11OpenGL/
#
# 2. Add option at top of this file:
#    option(VKING_BUILD_PLUGIN_X11_OPENGL "Build X11+OpenGL render plugin" OFF)
#
# 3. Add dependency analysis:
#    if(VKING_BUILD_PLUGIN_X11_OPENGL)
#        set(NEED_X11 ON)
#        set(NEED_OPENGL ON)
#    endif()
#
# 4. The build logic for X11 and OpenGL dependencies is already there
#    (or add it in the Windowing/RenderingBackends sections if new).
#
# 5. Add plugin build at bottom:
#    if(VKING_BUILD_PLUGIN_X11_OPENGL)
#        add_subdirectory(Glue/X11OpenGL)
#    endif()
#
# 6. In Glue/X11OpenGL/CMakeLists.txt:
#    - Build a SHARED library
#    - Link PRIVATE against VKING::Platform::X11 and OpenGL::OpenGL
#    - Install to plugins/Renderer/
# =============================================================================