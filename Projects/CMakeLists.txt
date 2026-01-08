# =============================================================================
# VKING Application Projects
# =============================================================================
# This CMakeLists.txt aggregates all executable applications built on top of
# the VKING Engine. Each subdirectory is a self-contained project that:
#   - Defines its own executable target
#   - Links against VKING::Engine (and optionally other libraries)
#   - May define its own C++23 modules for project-specific functionality
# =============================================================================

# =============================================================================
# Editor Project
# =============================================================================
# The VKING Editor is the primary development tool for the engine.
# It provides a GUI for scene editing, asset management, and debugging.
# Currently the only application project; others will be added here as needed.
# =============================================================================
add_subdirectory(Editor)

# =============================================================================
# Future Projects (examples)
# =============================================================================
# To add new applications, create a subdirectory and call add_subdirectory():
#
# add_subdirectory(Game)        # Example game project
# add_subdirectory(Tools)       # Asset pipeline tools
# add_subdirectory(TestHarness) # Automated testing framework
#
# Each project should follow the same pattern as Editor:
#   - Define an executable target
#   - Link PRIVATE against VKING::Engine
#   - Reuse PCH from VKING::Engine if desired
#   - Apply vking_apply_warnings() for consistency
# =============================================================================