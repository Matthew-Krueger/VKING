# Build the GLFW+Vulkan render plugin as a shared library
add_library(VKING_Render_GLFW_Vulkan SHARED
        GLFWVulkan.cpp
        # other plugin source files
)

# Link against dependencies (static libraries)
target_link_libraries(VKING_Render_GLFW_Vulkan PRIVATE
        VKING::Platform::GLFW    # From Windowing/GLFW/
        Vulkan::Vulkan           # From find_package(Vulkan)
        # Any other dependencies
)

# Set output name (optional but recommended)
set_target_properties(VKING_Render_GLFW_Vulkan PROPERTIES
        OUTPUT_NAME "VKING_Render_GLFW_Vulkan"
        PREFIX ""  # Avoid 'lib' prefix on Unix
)

# Install plugin to plugins/Renderer/
install(TARGETS VKING_Render_GLFW_Vulkan
        LIBRARY DESTINATION plugins/Renderer
        RUNTIME DESTINATION plugins/Renderer  # Windows DLLs
)

# Copy resources for development
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
    add_custom_command(TARGET VKING_Render_GLFW_Vulkan POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/Resources
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/GLFWVulkan
    )
endif()

add_library(VKING::Platform::Glue::GLFWVulkan ALIAS VKING_Render_GLFW_Vulkan)