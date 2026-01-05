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


export module VKING.Application;

import VKING.Log;

export namespace VKING {
    class Application {
    public:
        virtual ~Application() = default;

        /**
         * @brief Main Event Loop
         */
        void run();

    private:
    };

} // VKING


namespace VKING {

    void Application::run() {

        using clock = std::chrono::steady_clock;
        auto previousTime = clock::now();
        auto currentTime = previousTime;
        float deltaTime = 0.0f;

        while (!Shutdown::isRequested()) {

            currentTime = clock::now();
            deltaTime = std::chrono::duration<float, std::milli>(currentTime - previousTime).count();
            previousTime = currentTime;

            VKING::Shutdown::request(VKING::Shutdown::Reason::REASON_FATAL_ERROR, "No work to do");

        }

    }

}