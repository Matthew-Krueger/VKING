add_executable(VKING_Editor main.cpp)

target_link_libraries(VKING_Editor PRIVATE GLFW::GLFW glm::glm)

if(BUILD_WITH_OPENMP)
    target_link_libraries(VKING_Editor PRIVATE OpenMP::OpenMP_CXX VKING::Engine::SharedResources)
endif()

target_precompile_headers(VKING_Editor REUSE_FROM VKING::Engine::SharedResources)

add_executable(VKING_Editor::VKING_Editor ALIAS VKING_Editor)