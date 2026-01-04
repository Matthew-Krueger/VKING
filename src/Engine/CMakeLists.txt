add_subdirectory(platform)

add_library(VKING_Engine OBJECT
        Application.cpp
        Application.hpp

)

target_link_libraries(
        VKING_Engine
        PUBLIC
        VKING::Engine::SharedResources
)

target_precompile_headers(
        VKING_Engine
        REUSE_FROM
        VKING::Engine::SharedResources
)


add_library(VKING::Engine ALIAS VKING_Engine)