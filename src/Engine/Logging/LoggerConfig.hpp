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

/**
 * @file LoggerConfig.hpp
 * @brief Host-side logging configuration and initialization for VKING.
 *
 * This header declares the host-owned logging initialization routine and the
 * function that provides the stable logging ABI table (`VKING_Logging_API`).
 *
 * Important separation of concerns:
 * - The stable C ABI (function pointers) is defined in LoggerStableCABI.h.
 * - The host implements those function pointers (typically using spdlog) in a
 *   .cpp file (e.g., Logger.cpp).
 * - Plugins never include spdlog headers and never call spdlog directly. They
 *   only call the ABI function pointers provided by the host.
 *
 * Lifetime:
 * - The host owns the `VKING_Logging_API` table returned by `getLoggingAPISpec()`.
 * - The returned pointer is expected to remain valid for the lifetime of the
 *   process (or at least while any plugin may log).
 *
 * Initialization order:
 * - `Init()` should be called early during host startup (before loading plugins),
 *   so that ABI callbacks (e.g., `logMessage`) have a fully initialized backend.
 */

#pragma once

#include "LoggerStableCABI.h"

#include <string>

namespace VKING::Logger {
    /**
     * @brief Default log pattern used for both console and file output (spdlog-style).
     *
     * This pattern is intended for the host's logging backend configuration.
     *
     * Notes:
     * - `%^` and `%$` enable/disable color range for sinks that support coloring.
     * - `%Y-%m-%d %H:%M:%S.%f` prints a timestamp with microseconds.
     * - `%n` prints the logger name (host-side engine logger, e.g. "VKING").
     * - `%l` prints the log level.
     * - `%s:%#` prints source file and line number (as passed via ABI).
     * - `%v` prints the log message payload (which may already include a category
     *   prefix such as "[Renderer] ...", depending on host formatting policy).
     */
    static constexpr auto DEFAULT_LOG_PATTERN =
            "%^[%Y-%m-%d %H:%M:%S.%f] [%n] [%l] [%s:%#] %v %$";

    /**
     * @brief Default file path used for log output if the host does not specify one.
     */
    static constexpr auto DEFAULT_LOG_FILE_PATH = "VKING.log";

    /**
     * @brief Get the host-owned logging ABI function table.
     *
     * The returned table contains function pointers that plugins (and optionally
     * host-side convenience wrappers) can call to emit logs without linking against
     * the host logging backend library.
     *
     * @return Pointer to a host-owned `VKING_Logging_API` table.
     *
     * @note The returned pointer must remain valid for as long as plugins may call
     *       into it. A typical implementation returns the address of a function-
     *       static `VKING_Logging_API` object.
     */
    const VKING_Logging_API *getLoggingAPISpec();

    /**
     * @brief Initialize the host logging backend (e.g., spdlog sinks, levels, patterns).
     *
     * This function is intended to be called exactly once by the host application
     * during startup, before loading plugins or otherwise allowing logging calls.
     *
     * Responsibilities typically include:
     * - Creating console and file sinks.
     * - Setting global log level and per-sink levels.
     * - Configuring formatting/patterns.
     * - Creating/registering the engine logger instance (e.g., named "VKING").
     *
     * @param fileName Path to the log file.
     * @param initialLevel Initial global log level to configure.
     *
     * @warning This function is host-only. Plugins should not call it.
     */
    void Init(
        const std::string &fileName, VKING_Logging_Level initialLevel = VKING_LOG_INFO
    );
} // namespace VKING::Logger
