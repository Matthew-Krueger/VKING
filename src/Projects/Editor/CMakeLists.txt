add_executable(VKING_Editor main.cpp)

# -----------------------------------------------------------------------------
# Public C++23 modules
# -----------------------------------------------------------------------------
# These are PUBLIC because consumers need to be able to write:
#     import VKING.Engine.Math;
# in their own translation units.
# -----------------------------------------------------------------------------
target_sources(VKING_Editor
        PUBLIC
        FILE_SET CXX_MODULES TYPE CXX_MODULES
        FILES
        Application.ixx
)

target_link_libraries(VKING_Editor PRIVATE VKING::Engine)

target_precompile_headers(VKING_Editor REUSE_FROM VKING::SharedResources)

add_executable(VKING_Editor::VKING_Editor ALIAS VKING_Editor)