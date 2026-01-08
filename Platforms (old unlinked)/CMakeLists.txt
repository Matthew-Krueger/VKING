# Options to control building of components
option(VKING_ENABLE_VULKAN      "Enable Vulkan support"          ON)
option(VKING_ENABLE_METAL       "Enable Metal support"           OFF)
option(VKING_ENABLE_GNM         "Enable GNM support"             OFF)
option(VKING_ENABLE_OPENGL      "Enable OpenGL support"          OFF)
option(VKING_ENABLE_DIRECTX_12  "Enable DirectX 12 support"      OFF)

option(VKING_ENABLE_GLFW        "Enable GLFW support"            ON)
option(VKING_ENABLE_WAYLAND     "Enable Wayland support"         OFF)
option(VKING_ENABLE_X11         "Enable X11 support"             OFF)
option(VKING_ENABLE_COCOA       "Enable Cocoa support"           OFF)
option(VKING_ENABLE_WIN32       "Enable Win32 support"           OFF)

# Debug information
message(STATUS "VKING_ENABLE_VULKAN:      ${VKING_ENABLE_VULKAN}")
message(STATUS "VKING_ENABLE_METAL:       ${VKING_ENABLE_METAL}")
message(STATUS "VKING_ENABLE_GNM:         ${VKING_ENABLE_GNM}")
message(STATUS "VKING_ENABLE_OPENGL:      ${VKING_ENABLE_OPENGL}")
message(STATUS "VKING_ENABLE_DIRECTX_12:  ${VKING_ENABLE_DIRECTX_12}")

message(STATUS "VKING_ENABLE_GLFW:        ${VKING_ENABLE_GLFW}")
message(STATUS "VKING_ENABLE_WAYLAND:     ${VKING_ENABLE_WAYLAND}")
message(STATUS "VKING_ENABLE_X11:         ${VKING_ENABLE_X11}")
message(STATUS "VKING_ENABLE_COCOA:       ${VKING_ENABLE_COCOA}")
message(STATUS "VKING_ENABLE_WIN32:       ${VKING_ENABLE_WIN32}")


if(VKING_ENABLE_VULKAN)
    add_subdirectory(vulkan)
endif()
if(VKING_ENABLE_METAL)
    add_subdirectory(metal)
endif()
if(VKING_ENABLE_GNM)
    add_subdirectory(gnm)
endif()
if(VKING_ENABLE_OPENGL)
    add_subdirectory(opengl)
endif()
if(VKING_ENABLE_DIRECTX_12)
    add_subdirectory(directx12)
endif()

# Conditionally add platform subdirectories
if(VKING_ENABLE_GLFW)
    add_subdirectory(GLFW)
endif()
if(VKING_ENABLE_WAYLAND)
    add_subdirectory(wayland)
endif()
if(VKING_ENABLE_X11)
    add_subdirectory(x11)
endif()
if(VKING_ENABLE_COCOA)
    add_subdirectory(cocoa)
endif()
if(VKING_ENABLE_WIN32)
    add_subdirectory(win32)
endif()

# Create interface target for availability and linkage propagation
add_library(VKING_Platform_AvailableTargets INTERFACE)
add_library(VKING::Platform::AvailableTargets ALIAS VKING_Platform_AvailableTargets)

## Conditionally propagate the linkage of the static libraries
# === Backends ===
if(VKING_ENABLE_VULKAN)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_VULKAN=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Vulkan)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_VULKAN=0)
endif()

if(VKING_ENABLE_METAL)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_METAL=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Metal)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_METAL=0)
endif()

if(VKING_ENABLE_GNM)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GNM=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::GNM)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GNM=0)
endif()

if(VKING_ENABLE_OPENGL)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_OPENGL=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::OpenGL)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_OPENGL=0)
endif()

if(VKING_ENABLE_DIRECTX_12)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_DIRECTX_12=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::DirectX12)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_DIRECTX_12=0)
endif()

# === Platforms ===
if(VKING_ENABLE_GLFW)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::GLFW)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW=0)
endif()

if(VKING_ENABLE_WAYLAND)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_WAYLAND=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Wayland)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_WAYLAND=0)
endif()

if(VKING_ENABLE_X11)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_X11=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::X11)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_X11=0)
endif()

if(VKING_ENABLE_COCOA)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_COCOA=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Cocoa)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_COCOA=0)
endif()

if(VKING_ENABLE_WIN32)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_WIN32=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Win32)
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_WIN32=0)
endif()

# === Glue layers (only the combinations that actually exist) ===
# In this example only GLFW+Vulkan glue exists. Others are forced to 0.
if(VKING_ENABLE_VULKAN AND VKING_ENABLE_GLFW)
    add_subdirectory(Glue-GLFWVulkan)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW_VULKAN_GLUE=1)
    target_link_libraries(VKING_Platform_AvailableTargets INTERFACE VKING::Platform::Glue::GLFWVulkan)
    message(STATUS "Building Glue-GLFWVulkan")
else()
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_GLFW_VULKAN_GLUE=0)
endif()

# All other possible glue combinations are not supported → always 0
target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE
        VKING_HAS_GLFW_METAL_GLUE=0
        VKING_HAS_GLFW_GNM_GLUE=0
        VKING_HAS_GLFW_OPENGL_GLUE=0
        VKING_HAS_GLFW_DIRECTX_12_GLUE=0
        VKING_HAS_WAYLAND_VULKAN_GLUE=0
        VKING_HAS_X11_VULKAN_GLUE=0
        VKING_HAS_COCOA_METAL_GLUE=0
        VKING_HAS_WIN32_DIRECTX_12_GLUE=0
        # ... add more if you ever implement them
)

# === Final preprocessor checks (to be placed in a config header or directly included) ===
# You can put this block into a generated header (e.g., VKING_Config_Platform.h.in → VKING_Config_Platform.h)
# or keep it as a separate .h that is included where needed.

target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE
        VKING_HAS_AT_LEAST_ONE_PLATFORM=0
        VKING_HAS_AT_LEAST_ONE_BACKEND=0
        VKING_HAS_AT_LEAST_ONE_PLATFORM_BACKEND_GLUE=0
)

# Individual checks that flip the "at least one" flags
if(VKING_ENABLE_GLFW)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM=1)
endif()
if(VKING_ENABLE_WAYLAND)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM=1)
endif()
if(VKING_ENABLE_X11)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM=1)
endif()
if(VKING_ENABLE_COCOA)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM=1)
endif()
if(VKING_ENABLE_WIN32)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM=1)
endif()

if(VKING_ENABLE_VULKAN)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_BACKEND=1)
endif()
if(VKING_ENABLE_METAL)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_BACKEND=1)
endif()
if(VKING_ENABLE_GNM)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_BACKEND=1)
endif()
if(VKING_ENABLE_OPENGL)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_BACKEND=1)
endif()
if(VKING_ENABLE_DIRECTX_12)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_BACKEND=1)
endif()

# Glue "at least one" flag (only flips if any supported glue is enabled)
if(VKING_ENABLE_VULKAN AND VKING_ENABLE_GLFW)
    target_compile_definitions(VKING_Platform_AvailableTargets INTERFACE VKING_HAS_AT_LEAST_ONE_PLATFORM_BACKEND_GLUE=1)
endif()

# The compile-time errors (these can be enforced in a header included everywhere)
# Example header content:
#
# #if (VKING_HAS_AT_LEAST_ONE_PLATFORM == 0)
# #error "VKING Engine cannot be built: At least one platform (e.g., GLFW) must be enabled."
# #endif
#
# #if (VKING_HAS_AT_LEAST_ONE_BACKEND == 0)
# #error "VKING Engine cannot be built: At least one backend (e.g., Vulkan) must be enabled."
# #endif
#
# #if (VKING_HAS_AT_LEAST_ONE_PLATFORM_BACKEND_GLUE == 0)
# #error "VKING Engine cannot be built: At least one platform-backend glue (e.g., GLFW+Vulkan) must be enabled."
# #endif