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

export module VKING.Types.Window;

export namespace VKING::Types {
    class Window {
    public:
        struct WindowCreateInfo {
            std::string title;
            uint32_t width;
            uint32_t height;
        };


        virtual ~Window() = default;

        /**
         * @brief Handles a window callback for if the window requests to close
         *
         * @param The window that requested the close
         *
         * @return Whether or not the close request was handled. If it was handled return true, if not return false. This allows conditional subscription
         */
        using WindowCloseRequestCallbackEventFN = bool (*)(Window* window);

        /**
         * @brief Polls for events so the window remains responsive
         */
        virtual void pollEvents() = 0;

        /**
         * @brief Sets the window requests closure callback function
         *
         * @param callback the Callback function
         */
        virtual void setWindowCloseRequestCallbackEventFN(WindowCloseRequestCallbackEventFN callback) = 0;

        /**
         * @brief Gets the native window handle, type erased to void*
         *
         * @return The native window handle as a void*
         */
        virtual void* getNativeWindowHandle() = 0;

    protected:
        Window() = default;

    private:

    };
}