/*
 *         A high-performance, module-first game engine.
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
// Created by Matthew Krueger on 1/3/26.
//
module;
#include <chrono>
#include <thread>
#include "../include/VKING/Logger.hpp"


export module VKING.Application;

import VKING.Types.Platform;
import VKING.EngineConfig;
import VKING.Types.Window;

export namespace VKING {
    class Application {
    public:
        explicit Application();
        virtual ~Application() = default;

        /**
         * @brief Main Event Loop
         */
        void run();

    private:
        std::unique_ptr<VKING::Types::Platform::PlatformManager> m_PlatformManager;
        std::unique_ptr<Types::Window> m_Window;
    };

} // VKING


namespace VKING {

    Application::Application() {

        m_PlatformManager = VKING::EngineConfig::selectPlatform({.platformType = Types::Platform::PlatformType::PLATFORM_NO_PREFERENCE, .backendType = Types::Platform::BackendType::VULKAN});
        m_Window = m_PlatformManager->createWindow({"VKING Window", 1280, 720});
        m_Window->setWindowCloseRequestCallbackEventFN([]([[maybe_unused]] Types::Window* window) {
            VKING::Shutdown::request(Shutdown::Reason::REASON_USER_REQUEST, "Window Close Request Received.");
            return true;
        });
    }

    void Application::run() {

        using clock = std::chrono::steady_clock;
        auto previousTime = clock::now();
        auto currentTime = previousTime;
        float deltaTime = 0.0f;

        while (!Shutdown::isRequested()) {

            currentTime = clock::now();
            deltaTime = std::chrono::duration<float, std::milli>(currentTime - previousTime).count();
            previousTime = currentTime;

            // just while we don't have a real event loop or load, lets prevent the thread from infinitely sinning
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            //VKING::Shutdown::request(VKING::Shutdown::Reason::REASON_FATAL_ERROR, "No work to do");

            m_Window->pollEvents();

        }

    }

}