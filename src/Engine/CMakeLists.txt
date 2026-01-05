add_library(VKING_Engine STATIC
        Application.cpp
        Application.hpp

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


add_library(VKING::Engine ALIAS VKING_Engine)