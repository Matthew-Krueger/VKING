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

#pragma once

#include "LoggerStableCABI.h"

namespace VKING::Logger {

    /// Default log pattern used for both console and file output
    static constexpr auto DEFAULT_LOG_PATTERN = "%^[%Y-%m-%d %H:%M:%S.%f] [%n] [%l] [%s:%#] %v %$";
    /// Default file path for log output
    static constexpr auto DEFAULT_LOG_FILE_PATH = "VKING.log";

    const VKING_Logging_API *getLoggingAPISpec();

    void Init(const std::string& fileName, const VKING_Logging_Level initialLevel = VKING_LOG_INFO);

}