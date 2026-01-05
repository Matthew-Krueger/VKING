# Options to control building of components
option(VKING_ENABLE_VULKAN "Enable Vulkan support" OFF)
option(VKING_ENABLE_GLFW "Enable GLFW support" OFF)

# Debug information
message(STATUS "VKING_ENABLE_VULKAN: ${VKING_ENABLE_VULKAN}")
message(STATUS "VKING_ENABLE_GLFW: ${VKING_ENABLE_GLFW}")

# Conditionally build Vulkan
if(VKING_ENABLE_VULKAN)
    add_subdirectory(vulkan)
endif()

# Conditionally build GLFW
if(VKING_ENABLE_GLFW)
    add_subdirectory(GLFW)
endif()

# Conditionally build Glue-GLFWVulkan if both are enabled
if(VKING_ENABLE_VULKAN AND VKING_ENABLE_GLFW)
    add_subdirectory(Glue-GLFWVulkan)
    message(STATUS "Building Glue-GLFWVulkan since both Vulkan and GLFW are enabled")
else()
    message(STATUS "Skipping Glue-GLFWVulkan because not both Vulkan and GLFW are enabled")
endif()

# Create interface target for availability and linkage propagation
add_library(VKING_Platform_AvailableTargets INTERFACE)
add_library(VKING::Platform::AvailableTargets ALIAS VKING_Platform_AvailableTargets)

## Conditionally propagate the linkage of the static libraries

# Conditionally define and link for Vulkan
if(VKING_ENABLE_VULKAN)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_VULKAN=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Vulkan)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_VULKAN=0)
endif()

# Conditionally define and link for GLFW
if(VKING_ENABLE_GLFW)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::GLFW)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW=0)
endif()

# Conditionally define and link for Glue-GLFWVulkan
if(VKING_ENABLE_VULKAN AND VKING_ENABLE_GLFW)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW_VULKAN_GLUE=1)
    # We want to propagate this linkage
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Glue::GLFWVulkan)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW_VULKAN_GLUE=0)
endif()