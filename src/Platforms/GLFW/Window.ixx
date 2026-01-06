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

export module VKING.Platform.GLFW:Window;
import VKING.Types.Window;

namespace VKING::Platform::GLFW {
    export class Window final : public Types::Window {
    public:
        struct WindowCreateInfo {
            /**
             * @brief Call glfwWindowHint() within this function to set the glfw state machine
             */
            void (*pfn_ApplyWindowCreationHints)(const WindowCreateInfo &)  = nullptr;

            /**
             * @brief Call make context current within this function if needed. If not, leave this blank
             */
            void (*pfn_ContextCurrentCallback)()  = nullptr;

            /**
             * @brief The window title
             */
            std::string title;
            uint32_t width, height;

            /**
             * @brief If the window is immediately visible.
             */
            bool visible = true;
        };

        explicit Window(const WindowCreateInfo& createInfo);
        ~Window() override;

        void setWindowCloseRequestCallbackEventFN(WindowCloseRequestCallbackEventFN callback) override {};
        void* getNativeWindowHandle() override { return m_GLFWwindow; }

        void pollEvents() override;

    private:
        GLFWwindow* m_GLFWwindow = nullptr;
        Types::Window::WindowCloseRequestCallbackEventFN m_WindowCloseRequestCallbackEventFN = nullptr; // don't *need* to store it here but good to do
        std::string m_Title;
        int m_Width = 0, m_Height = 0;
        bool m_Visible = false;

    };
}