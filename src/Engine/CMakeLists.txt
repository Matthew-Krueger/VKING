add_library(VKING_Engine STATIC
        Application.ixx
        include/VKING/MainCreator.hpp
        EntryPoint.cpp

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
)

target_link_libraries(
        VKING_Engine
        PRIVATE VKING::Platform::AvailableTargets
        PUBLIC VKING::SharedResources
)

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



add_library(VKING::Engine ALIAS VKING_Engine)