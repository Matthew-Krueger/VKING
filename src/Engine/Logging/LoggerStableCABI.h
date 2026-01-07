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

#include "../Plugins/PluginVKING_CALLDef.h"

// Everything inside extern "C" is stable for use in plugins
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

typedef enum VKING_Logging_Level {
    VKING_LOG_TRACE = 0,
    VKING_LOG_DEBUG = 1,
    VKING_LOG_INFO = 2,
    VKING_LOG_WARN = 3,
    VKING_LOG_ERROR = 4,
    VKING_LOG_CRITICAL = 5,
    VKING_LOG_OFF = 6
} VKING_Logging_Level;

typedef struct VKING_Logging_API {
    uint32_t abiVersion;
    uint32_t structSize;

    void (VKING_CALL*logMessage)(
        VKING_Logging_Level level,
        const char *logger, /* e.g. "EngineConfig" */
        const char *file,
        int32_t line,
        const char *function,
        const char *message
    );

    VKING_Logging_Level (VKING_CALL*getGlobalLogLevel)();
    void (VKING_CALL*setGlobalLogLevel)(VKING_Logging_Level level);
} VKING_Logging_API;

#ifdef __cplusplus
}
#endif
