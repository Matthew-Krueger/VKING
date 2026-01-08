# =============================================================================
# VKING Engine Core Library
# =============================================================================
# This target contains the main engine code that will be compiled into a static
# library. It provides both traditional C++ headers and C++23 modules for
# consumption by applications and plugins.
# =============================================================================

# =============================================================================
# VKING Engine EntryPoint (Main Functions)
# =============================================================================
# Include the Main Functions Library
# To use, link with either VKING::EntryPoint::GUI or VKING::EntryPoint::Console
# =============================================================================
add_subdirectory(EntryPointLib)

# Static library containing all engine implementation files.
# Files listed here are compiled directly into the library binary.
add_library(VKING_Engine STATIC
        # Main application module interface
        Application.ixx

        # Entry point implementation (main() function wrapper)
        EngineEntryPoint.cpp

        # Logger service implementation - compiled into engine binary
        # This is PRIVATE because plugins access logging through the C ABI, not this TU
        Services/Logger/Logger.cpp

        # Logger template header - provides inline wrappers for plugin logging
        # Included in SDK so plugins can use typed logging without spdlog
        SDK/Logger/LoggerTemplates.hpp

        # C ABI specification for host-side logging service
        # Defines the stable interface that plugins call into for logging
        SDK/ABI/HostLoggingABISpec.h

        # C ABI specification for host-to-plugin communication
        # Defines structures used by plugins to interact with host services
        SDK/ABI/VKINGHostToPluginABI.h

        # Meta-header that includes all plugin ABI specifications
        # Plugins include this single header to get all C interface definitions
        include/VKING/SDK/ABI/CPluginInterface.h

        # Public C++ SDK header for logging
        # Exposes engine logging API to applications (not plugins)
        include/VKING/SDK/Logger.hpp

        # Calling convention and export/import macros for plugin boundary
        # Used by both host and plugins to ensure ABI stability across compilers
        SDK/ABI/PluginVKING_CALLDef.h

        # Signal/slot system implementation
        Signals/Signals.cpp
        Signals/Signals.hpp

        # Scoring utility for platform/backend selection
        # Used by plugin loader to rank available render plugins
        Types/Scored.hpp

        # Host-side logger initialization and management
        # Sets up spdlog backend and provides C ABI function pointers
        Services/Logger/LoggerHost.hpp

        # C ABI specification for render plugin interface
        # Defines the function table that render plugins export
        SDK/ABI/RenderABISpec.h
)

# =============================================================================
# Public C++23 Modules
# =============================================================================
# These modules are PUBLIC because they form part of the engine's API.
# External code (applications, tools) can import these modules.
# The FILE_SET mechanism tells CMake these are C++20+ module interface files.
# =============================================================================
target_sources(VKING_Engine
        PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES
        FILES
        # Main application module - defines VKING::Application
        Application.ixx

        # Entry point callbacks for application lifecycle
        EntryPointCallbacks.ixx

        # Plugin host services provider
        # Exposes host-side APIs (logging, etc.) to plugins
        PluginHost/PluginHost.ixx

        # Render plugin loader and manager
        # Handles discovery, loading, and lifecycle of render plugins
        PluginHost/Render/RenderPluginLoader.ixx
)

# =============================================================================
# Link Dependencies
# =============================================================================
# PRIVATE: Dependencies only used internally by the engine
# PUBLIC: Dependencies that propagate to consumers of the engine library
# =============================================================================
target_link_libraries(
        VKING_Engine
        PRIVATE
        # spdlog is private because plugins use C ABI logging, not direct spdlog
        spdlog::spdlog_header_only
        fmt::fmt-header-only
        PUBLIC
        # fmt is public because engine headers expose fmt types
        fmt::fmt-header-only
        # glm is public because engine headers expose glm types
        glm::glm
)

# =============================================================================
# Compile Definitions
# =============================================================================
# SPDLOG_FMT_EXTERNAL: Tells spdlog to use external fmt library instead of bundled
# SPDLOG_FMT_EXTERNAL_HO: Header-only version of the above
# These must be PRIVATE to avoid leaking spdlog macros into plugin code
# =============================================================================
target_compile_definitions(VKING_Engine PRIVATE SPDLOG_FMT_EXTERNAL_HO SPDLOG_FMT_EXTERNAL)

# =============================================================================
# Precompiled Headers
# =============================================================================
# Prerequisites.hpp contains GLM config and STL includes
# PRIVATE because it's an implementation detail - plugins must include GLMConfig manually
# This significantly reduces compile times for engine TUs (~15 seconds → ~2 seconds)
# =============================================================================
target_precompile_headers(
        VKING_Engine
        PRIVATE
        include/VKING/SDK/Prerequisites.hpp
)

# =============================================================================
# Include Directories
# =============================================================================
# BUILD_INTERFACE: Path used when building the engine itself
# INSTALL_INTERFACE: Path used by external projects linking against installed engine
# This allows #include <VKING/...> to work both in-tree and when installed
# =============================================================================
target_include_directories(VKING_Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =============================================================================
# Compiler Warnings
# =============================================================================
# Applies project-wide warning settings defined in VKING_Warnings.cmake
# This ensures consistent warning levels across all engine code
# =============================================================================
vking_apply_warnings(VKING_Engine)

# =============================================================================
# Alias Target
# =============================================================================
# Creates VKING::Engine as an alias for VKING_Engine
# Allows consumers to use find_package(VKING) and link against VKING::Engine
# Follows modern CMake best practices for namespaced targets
# =============================================================================
add_library(VKING::Engine ALIAS VKING_Engine)