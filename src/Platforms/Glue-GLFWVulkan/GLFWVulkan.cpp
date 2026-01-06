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
// Created by Matthew Krueger on 1/6/26.
//
module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


module VKING.Platform.Glue.GLFWVulkan;

import VKING.Types.Platform;
import VKING.Types.Window;
import VKING.Platform.GLFW;

namespace VKING::Platform::Glue {

    std::unique_ptr<Types::Window> GLFWVulkan::createWindow(const Types::Window::WindowCreateInfo &createInfo) {

        PlatformGLFWVulkanLogger::record().debug("Creating GLFW window.");

        // create the window
        GLFW::Window::WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = createInfo.title;
        windowCreateInfo.width = createInfo.width;
        windowCreateInfo.height = createInfo.height;
        windowCreateInfo.visible = true;
        windowCreateInfo.pfn_ApplyWindowCreationHints = [](const GLFW::Window::WindowCreateInfo &windowCreateInfo) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, windowCreateInfo.visible ? GLFW_TRUE : GLFW_FALSE);
        };

        return std::make_unique<GLFW::Window>(windowCreateInfo);

    }

    std::unique_ptr<Types::Platform::RHI> GLFWVulkan::createRHI() {

        PlatformGLFWVulkanLogger::record().info("Creating Vulkan RHI.");
        return nullptr;

    }


}
