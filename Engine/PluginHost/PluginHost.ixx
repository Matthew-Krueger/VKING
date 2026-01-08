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
// Created by Matthew Krueger on 1/7/26.
//
module;
#include <VKING/SDK/ABI/CPluginInterface.h>

#include "../Services/Logger/LoggerHost.hpp"
export module VKING.PluginHost;


export namespace VKING::PluginHost {
    /**
     * @brief Gets the host specification for plugins
     * @return The Plugin Host specification, the ABI spec.
     */
    const VKING_Hostside_API *getHostsideABISpec() {
        static const VKING_Hostside_API api = {
            .abiVersion = 1,
            .structSize = sizeof(VKING_Hostside_API),
            .loggingAPISpec = Logger::Host::hostsideGetLoggingAPISpec()
        };

        return &api;

    }

}