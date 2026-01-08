# Platforms/Windowing/GLFW/CMakeLists.txt
add_library(VKING_Platform_GLFW STATIC
        GLFWWrapper.cpp  # Your wrapper code
)
target_link_libraries(VKING_Platform_GLFW PRIVATE glfw)

add_library(VKING::Platform::GLFW ALIAS VKING_Platform_GLFW)