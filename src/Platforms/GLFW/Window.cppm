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

module VKING.Platform.GLFW:WindowImpl;
import :Window;
import :Logger;
import :Callbacks;

namespace VKING::Platform::GLFW {

    static std::atomic_int s_WindowCount = 0;

    Window::Window(const WindowCreateInfo& createInfo){
        m_Title = createInfo.title;
        m_Width = static_cast<int>(createInfo.width);
        m_Height = static_cast<int>(createInfo.height);
        m_Visible = createInfo.visible;

        // before we call any glfw functions, we have to init it
        // but we should only do so if we need to
        // AND we need to set the error callback
        glfwSetErrorCallback(VKING_Platform_GLFW_ErrorCallback);
        if (s_WindowCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
            ModuleLogger::record().trace("GLFW initializing. First time creating a window.");
            glfwInit();
            ModuleLogger::record().debug("GLFW initialized.");
        }

        // apply window creation hints, if any
        if (createInfo.pfn_ApplyWindowCreationHints) createInfo.pfn_ApplyWindowCreationHints(createInfo);

        ModuleLogger::record().debug("Creating GLFW window.");
        m_GLFWwindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (!m_GLFWwindow) {
            ModuleLogger::record().critical("GLFW window creation failed.");
        }

        // apply context queue if required
        if (createInfo.pfn_ContextCurrentCallback) createInfo.pfn_ContextCurrentCallback();

    }

    Window::~Window() {
        glfwDestroyWindow(m_GLFWwindow);
        if (s_WindowCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            glfwTerminate();
            ModuleLogger::record().debug("GLFW terminated, last window closed");
        }
        ModuleLogger::record().debug("GLFW window destroyed.");


    }

    void Window::pollEvents() {
        glfwPollEvents();
    }
}
