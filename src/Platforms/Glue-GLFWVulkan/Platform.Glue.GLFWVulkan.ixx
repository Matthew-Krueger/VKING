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
// Created by Matthew Krueger on 1/4/26.
//
module;
#include "../../../Engine/include/VKING/Logger.hpp"


export module VKING.Platform.Glue.GLFWVulkan;

import VKING.Types.Platform;
import VKING.Types.Window;

using PlatformGLFWVulkanLogger = VKING::Logger::Named<"PlatformCreator">;

namespace VKING::Platform::Glue {
    export class GLFWVulkan final : public Types::Platform::PlatformManager {
        /**
         * Constructs an instance of GLFWVulkan, a specialized platform manager for the Vulkan backend using GLFW.
         *
         * @param pfn_CreateFunctionArchive A function pointer used to create the platform-specific function archive.
         *                                   This is passed to the base PlatformManager constructor as part of the
         *                                   platform creation info.
         * @return A fully initialized instance of GLFWVulkan.
         */
    public:
        explicit GLFWVulkan(const Types::Platform::PlatformManager::PlatformSpecification::PlatformCreateInfo createInfo) : PlatformManager(Types::Platform::BackendType::VULKAN, Types::Platform::PlatformType::GLFW, createInfo){};

        std::unique_ptr<Types::Window> createWindow(const Types::Window::WindowCreateInfo &windowCreateInfo) override;

        std::unique_ptr<Types::Platform::RHI> createRHI() override;

    private:

    };

}

export extern "C" void VKING_Platform_Glue_GLFWVulkan_Destroy(
    VKING::Types::Platform::PlatformManager* p
) {
    delete p;
}

// now, we create a c linkage specifically to create this object
export extern "C" VKING::Types::Platform::PlatformManager* VKING_Platform_Glue_GLFWVulkan_Create(){
    PlatformGLFWVulkanLogger::record().debug("Invoked the GLFWVulkan GLUE LIBRARY Create Function");
    return new VKING::Platform::Glue::GLFWVulkan({VKING_Platform_Glue_GLFWVulkan_Create, VKING_Platform_Glue_GLFWVulkan_Destroy});
}

