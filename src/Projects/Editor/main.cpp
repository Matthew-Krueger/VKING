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

#define VKING_SUPPRESS_ENTRY_POINT_MESSAGES
#define VKING_INCLUDE_POSIX_MAIN
#define VKING_INCLUDE_WIN_MAIN
#include <VKING/MainCreator.hpp>

import VKING.Editor.Application;
import VKING.Log;

void VKING::registerLogger() {
    Log::Init("VKING-Editor.log");
    Log::setLevel(Log::Level::debug);
}


void* VKING::createApplication() {
    return new Editor::EditorApplication();
}