/*
 *         VKING: A high-performance, module-first game engine.
 *         Copyright (C) 2026 Matthew Krueger
 *
 *         This program is free software: you can redistribute it and/or modify
 *         it under the terms of the GNU General Public License as published by
 *         the Free Software Foundation, either version 3 of the License, or
 *         (at your option) any later version.
 *
 *         This program is distributed in the hope that it will be useful,
 *         but WITHOUT ANY WARRANTY; without even the implied warranty of
 *         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *         GNU General Public License for more details.
 *
 *         You should have received a copy of the GNU General Public License
 *         along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by Matthew Krueger on 1/7/26.
//

#ifndef VKING_RENDER_ABI_SPEC_HEADER
#define VKING_RENDER_ABI_SPEC_HEADER

#include <stdint.h>
#include "../PluginVKING_CALLDef.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VKING_PLUGIN_RENDER_ABI_VERSION_1 1u

typedef uint32_t VKING_Plugin_Render_BackendType;
enum {
    VKING_PLUGIN_RENDER_BACKEND_VULKAN = 0,
    VKING_PLUGIN_RENDER_BACKEND_METAL = 1,
    VKING_PLUGIN_RENDER_BACKEND_GNM = 2,
    VKING_PLUGIN_RENDER_BACKEND_OPENGL = 3,
    VKING_PLUGIN_RENDER_BACKEND_DIRECTX_12 = 4,
    VKING_PLUGIN_RENDER_BACKEND_UNSUPPORTED = 5,
    VKING_PLUGIN_RENDER_BACKEND_NO_PREFERENCE = 6
};

typedef uint32_t VKING_Plugin_Render_PlatformType;
enum {
    VKING_PLUGIN_RENDER_PLATFORM_GLFW = 0,
    VKING_PLUGIN_RENDER_PLATFORM_WAYLAND = 1,
    VKING_PLUGIN_RENDER_PLATFORM_X11 = 2,
    VKING_PLUGIN_RENDER_PLATFORM_COCOA = 3,
    VKING_PLUGIN_RENDER_PLATFORM_WIN32 = 4,
    VKING_PLUGIN_RENDER_PLATFORM_UNSUPPORTED = 5,
    VKING_PLUGIN_RENDER_PLATFORM_NO_PREFERENCE = 6
};

typedef struct VKING_Plugin_Render_Platform VKING_Plugin_Render_Platform;
typedef VKING_Plugin_Render_Platform* VKING_Plugin_Render_Platform_Handle;

typedef struct VKING_Plugin_Render_RHI VKING_Plugin_Render_RHI;
typedef VKING_Plugin_Render_RHI* VKING_Plugin_Render_RHI_Handle;

typedef struct VKING_Plugin_Render_Window VKING_Plugin_Render_Window;
typedef VKING_Plugin_Render_Window* VKING_Plugin_Render_Window_Handle;

typedef struct VKING_Plugin_Render_Swapchain VKING_Plugin_Render_Swapchain;
typedef VKING_Plugin_Render_Swapchain* VKING_Plugin_Render_Swapchain_Handle;

typedef struct VKING_Plugin_Render_WindowCreateInfo {
    uint32_t structSize;
    uint32_t width;
    uint32_t height;
    const char* title;
} VKING_Plugin_Render_WindowCreateInfo;

typedef struct VKING_Plugin_Render_ABIspec {
    uint32_t abiVersion;
    uint32_t structSize;

    uint32_t pluginVersion;
    const char* pluginName;

    VKING_Plugin_Render_BackendType backendType;
    VKING_Plugin_Render_PlatformType platformType;

    VKING_Plugin_Render_Platform_Handle(VKING_CALL* pfn_CreatePlatform)(void);
    void (VKING_CALL* pfn_DestroyPlatform)(VKING_Plugin_Render_Platform_Handle);

    VKING_Plugin_Render_Window_Handle(VKING_CALL* pfn_CreateWindow)(
        VKING_Plugin_Render_Platform_Handle,
        const VKING_Plugin_Render_WindowCreateInfo*
    );
    void (VKING_CALL* pfn_DestroyWindow)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_Window_Handle
    );
    void (VKING_CALL* pfn_PollEvents)(VKING_Plugin_Render_Platform_Handle);

    VKING_Plugin_Render_RHI_Handle(VKING_CALL* pfn_CreateRHI)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_Window_Handle
    );
    void (VKING_CALL* pfn_DestroyRHI)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_RHI_Handle
    );

    VKING_Plugin_Render_Swapchain_Handle(VKING_CALL* pfn_CreateSwapchain)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_RHI_Handle,
        VKING_Plugin_Render_Window_Handle
    );
    void (VKING_CALL* pfn_DestroySwapchain)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_RHI_Handle,
        VKING_Plugin_Render_Swapchain_Handle
    );
    void (VKING_CALL* pfn_PresentSwapchain)(
        VKING_Plugin_Render_Platform_Handle,
        VKING_Plugin_Render_RHI_Handle,
        VKING_Plugin_Render_Swapchain_Handle
    );

} VKING_Plugin_Render_ABIspec;

VKING_EXPORT const VKING_Plugin_Render_ABIspec* VKING_CALL
VKING_GetRenderPluginAPI(uint32_t requestedAbiVersion);

#define VKING_PLUGIN_RENDER_ABI_V1_MIN_STRUCT_SIZE \
(offsetof(VKING_Plugin_Render_ABIspec, pfn_PresentSwapchain) + \
sizeof(((VKING_Plugin_Render_ABIspec*)0)->pfn_PresentSwapchain))

#ifdef __cplusplus
}
#endif

#endif