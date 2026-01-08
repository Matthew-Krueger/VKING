# Platforms/RenderingBackends/Vulkan/CMakeLists.txt
add_library(VKING_Platform_Vulkan STATIC
        VulkanWrapper.cpp  # Your wrapper code
)
target_link_libraries(VKING_Platform_Vulkan PRIVATE Vulkan::Vulkan)

add_library(VKING::Platform::Vulkan ALIAS VKING_Platform_Vulkan)