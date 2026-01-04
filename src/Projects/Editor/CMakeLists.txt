add_executable(VKING_Editor main.cpp)

target_link_libraries(VKING_Editor PRIVATE VKING::Engine::SharedResources)

target_precompile_headers(VKING_Editor REUSE_FROM VKING::Engine::SharedResources)

add_executable(VKING_Editor::VKING_Editor ALIAS VKING_Editor)