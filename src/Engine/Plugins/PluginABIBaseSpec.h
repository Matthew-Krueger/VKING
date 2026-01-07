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

#include "../Logging/LoggerStableCABI.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    // we need to define our abi spec
    typedef struct VKING_ABI_SPEC {
        /**
        * @brief The ABI version the engine is exposing
        */
        uint32_t abiVersion;

        /**
         * @brief Size of the ABI struct
         */
        uint32_t structSize;

        /**
         * @brief The spec for the logging abi
         */
        const VKING_Logging_API *loggingAPISpec;

    } VKING_ABI_SPEC;

#ifdef __cplusplus
}
#endif
