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

#define VKING_SUPPRESS_ENTRY_POINT_MESSAGES
#define VKING_INCLUDE_POSIX_MAIN
#define VKING_INCLUDE_WIN_MAIN
#include <VKING/MainCreator.hpp>

#define VKING_INCLUDE_LOGGING_CONFIG
#include <VKING/Logger.hpp>

module VKING.EntryPointCallbacks;

import VKING.Editor.Application;

using EditorMainLogger = VKING::Logger::Named<"EditorMain">;

VKING::Logger::CreateInfo VKING::getLoggerConfig() {
    return{
        "VKING-Editor.log",
        Logger::Level::VKING_LOG_TRACE
    };
}

std::unique_ptr<VKING::Application> VKING::createApplication() {
    EditorMainLogger::record().info("Creating application.");
    return std::make_unique<Editor::EditorApplication>();
}

