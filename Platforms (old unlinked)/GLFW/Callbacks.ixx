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
#include <GLFW/glfw3.h>

export module VKING.Platform.GLFW:Callbacks;

import :Logger;
import VKING.Types.Window;

extern "C" {

    export void VKING_Platform_GLFW_ErrorCallback(int error, const char* description) {
        VKING::Platform::GLFW::ModuleLogger::record().error("[GLFW Error Callback] GLFW Error: {} - {}", error, description);
    }

    export void VKING_Platform_GLFW_WindowCloseCallback(GLFWwindow *window) {

        VKING::Platform::GLFW::ModuleLogger::record().debug("[GLFW Window Close Callback] GLFW Window Close Callback triggered");

        const auto userWindow = static_cast<VKING::Types::Window *>(glfwGetWindowUserPointer(window));
        // I am pretty sure this is evil since technically we're casting away the lifetime of the window object
        // since it's inside of a lifetime.
        if (const auto callbackFN = userWindow->getWindowCloseRequestCallbackEventFN()) callbackFN(userWindow);
    }

}
