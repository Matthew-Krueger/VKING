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

#ifndef VKING_LOGGER_STABLE_CABI_H
#define VKING_LOGGER_STABLE_CABI_H

#include <stdint.h>

#include "../PluginVKING_CALLDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum VKING_Logging_Level
 * @brief Log severity levels used by the VKING logging ABI.
 *
 * These values are intentionally engine-defined (not spdlog-defined) to keep the
 * ABI independent of any particular logging library.
 */
typedef enum VKING_Logging_Level {
    /** Detailed trace events (very verbose). */
    VKING_LOG_TRACE = 0,
    /** Debug information useful during development. */
    VKING_LOG_DEBUG = 1,
    /** General informational messages. */
    VKING_LOG_INFO = 2,
    /** Warnings indicating potential issues. */
    VKING_LOG_WARN = 3,
    /** Errors indicating an operation failed. */
    VKING_LOG_ERROR = 4,
    /** Critical/fatal errors requiring attention. */
    VKING_LOG_CRITICAL = 5,
    /** Logging disabled. */
    VKING_LOG_OFF = 6
} VKING_Logging_Level;

/**
 * @struct VKING_Hostside_Logging_API
 * @brief Function table for logging services provided by the host to plugins.
 *
 * Plugins should treat this structure as read-only.
 *
 * Versioning:
 * - `abiVersion` identifies the ABI major version of this table.
 * - `structSize` is the size in bytes of the table instance provided by the host.
 *   This allows newer hosts to extend the structure by appending new fields.
 *
 * Forward compatibility guideline for plugins:
 * - Validate `abiVersion` matches the version the plugin expects.
 * - Validate `structSize` is at least large enough to contain the fields the
 *   plugin intends to use.
 */
typedef struct VKING_Hostside_Logging_API {
    /**
     * @brief ABI version of this logging API table.
     *
     * Plugins should reject tables with an unexpected ABI version.
     */
    uint32_t abiVersion;

    /**
     * @brief Size in bytes of the `VKING_Logging_API` table instance provided by the host.
     *
     * Plugins may use this to verify required fields exist when supporting multiple
     * host versions.
     */
    uint32_t structSize;

    /**
     * @brief Emit a log message.
     *
     * @param level Log severity.
     * @param logger Category/tag for the message (e.g. "Renderer", "EngineConfig").
     *              This is typically derived from a compile-time logger name.
     * @param file Source file name (UTF-8), usually from `__FILE__` or
     *             `std::source_location::file_name()`.
     * @param line Source line number.
     * @param function Function name (UTF-8), e.g. `__func__` or
     *                 `std::source_location::function_name()`.
     * @param message Fully formatted message text (UTF-8).
     *
     * @note All string pointers must remain valid for the duration of the call only.
     *       The host must copy if it needs longer lifetime.
     */
    void (VKING_CALL*logMessage)(
        VKING_Logging_Level level,
        const char *logger,
        const char *file,
        int32_t line,
        const char *function,
        const char *message
    );

    /**
     * @brief Get the host's current global log level.
     *
     * @return Current global log level.
     */
    VKING_Logging_Level (VKING_CALL*getGlobalLogLevel)(void);

    /**
     * @brief Set the host's global log level.
     *
     * @param level New global log level.
     */
    void (VKING_CALL*setGlobalLogLevel)(VKING_Logging_Level level);
} VKING_Hostside_Logging_API;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VKING_LOGGER_STABLE_CABI_H */
