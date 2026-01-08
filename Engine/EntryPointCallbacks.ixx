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
// Created by Matthew Krueger on 1/5/26.
//
module;
#include "include/VKING/SDK/Logger.hpp"

export module VKING.EntryPointCallbacks;
import VKING.Application;

export namespace VKING {
    /**
     * @brief Creates and returns a unique pointer to an instance of VKING::Application.
     *
     * You create this by defining a function in YOUR application module that matches
     * the signature `std::unique_ptr<VKING::Application> VKING::createApplication()`.
     * This function is invoked by `VKING_Main` as a callback to instantiate your application.
     *
     * @note This function returns a `std::unique_ptr` to ensure type safety and proper
     * ownership semantics across module boundaries, leveraging C++20 modules for a
     * robust and modern entry point.
     *
     * @return A `std::unique_ptr` owning an instance of `VKING::Application`.
     */
    extern std::unique_ptr<Application> createApplication();

    /**
     * @brief Registers your application's logger with the VKING engine.
     *
     * You create this by defining a `void VKING::registerLogger()` function in YOUR
     * application module. It is invoked by `VKING_Main` as a callback *before*
     * `createApplication()` is called, ensuring logging is available from the earliest
     * stages of engine initialization.
     *
     * The engine will respect your logger's previously set log levels and will not
     * change them, with the exception of explicit lifecycle events (like startup,
     * shutdown, or interrupt handling). During these engine-controlled phases, the
     * engine may temporarily set its own log level, which is then restored to your
     * preference once control returns to the application or after the lifecycle event concludes.
     *
     * @note This function has a `void` return type, as the logger registration
     * typically involves internal calls within the `registerLogger` implementation
     * (e.g., `Log::Init()`).
     *
     */

    extern Logger::CreateInfo getLoggerConfig();

}