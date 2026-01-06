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

export module VKING.Platform.Glue.GLFWVulkan;

import VKING.Types.Platform;
import VKING.Log;

using PlatformGLFWVulkanLogger = VKING::Log::Named<"PlatformCreator">;

namespace VKING::Platform::Glue {
    class GLFWVulkan final : public Types::Platform::PlatformManager {
    public:
        explicit GLFWVulkan(const Types::Platform::CreateFn pfn_CreateFunctionArchive) : PlatformManager(Types::Platform::BackendType::VULKAN, Types::Platform::PlatformType::GLFW, PlatformSpecification::PlatformCreateInfo{pfn_CreateFunctionArchive}){};

        std::unique_ptr<Types::Platform::Window> createWindow() override;

        std::unique_ptr<Types::Platform::RHI> createRHI() override;

    private:

    };

    std::unique_ptr<Types::Platform::Window> GLFWVulkan::createWindow() {
        return nullptr;
    }

    std::unique_ptr<Types::Platform::RHI> GLFWVulkan::createRHI() {
        return nullptr;
    }

}

// now, we create a c linkage specifically to create this object
export extern "C" VKING::Types::Platform::PlatformManager* VKING_Platform_Glue_GLFWVulkan_Create(){
    PlatformGLFWVulkanLogger::record().trace("Invoked the GLFWVulkan Create Function");
    return new VKING::Platform::Glue::GLFWVulkan(VKING_Platform_Glue_GLFWVulkan_Create);
}