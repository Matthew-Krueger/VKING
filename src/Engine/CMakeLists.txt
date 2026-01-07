add_library(VKING_Engine STATIC
        Application.ixx
        include/VKING/MainCreator.hpp
        EntryPoint.cpp
        Config/ConfigFns.cpp
        Logging/Logger.cpp
        Logging/LoggerTemplates.hpp
        Logging/LoggerStableCABI.h
        Logging/LoggerConfig.hpp
        Plugins/PluginABIBaseSpec.h
        include/VKING/CPluginInterface.h
        include/VKING/Logger.hpp
        Plugins/PluginVKING_CALLDef.h
)

# -----------------------------------------------------------------------------
# Public C++23 modules
# -----------------------------------------------------------------------------
# These are PUBLIC because consumers need to be able to write:
#     import VKING.Engine.Math;
# in their own translation units.
# -----------------------------------------------------------------------------
target_sources(VKING_Engine
        PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES
        FILES
        Application.ixx
        EntryPointCallbacks.ixx
        Config/ConfigConstants.ixx
        Plugins/PluginHost.ixx
)

target_link_libraries(
        VKING_Engine
        PRIVATE VKING::Platform::AvailableTargets spdlog::spdlog_header_only fmt::fmt-header-only
        PUBLIC VKING::SharedResources fmt::fmt-header-only
)


target_compile_definitions(VKING_Engine PRIVATE SPDLOG_FMT_EXTERNAL_HO SPDLOG_FMT_EXTERNAL)

target_precompile_headers(
        VKING_Engine
        REUSE_FROM
        VKING::SharedResources
)

target_include_directories(VKING_Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # $<INSTALL_INTERFACE:include>  # Uncomment if you ever install the engine
)

vking_apply_warnings(VKING_Engine)


add_library(VKING::Engine ALIAS VKING_Engine)