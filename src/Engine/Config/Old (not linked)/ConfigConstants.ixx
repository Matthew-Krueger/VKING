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

export module VKING.EngineConfig;

import VKING.Types.Platform;

namespace VKING::EngineConfig {


    /**
     * @brief Selects the best matching platform manager based on the desired specification.
     *
     * This function evaluates the available platform configurations, attempting to find
     * an exact match for the specified platform and backend type based on the provided
     * `PlatformSpecification`. If an exact match is not available, it selects the best
     * available configuration based on a scoring system. The selected platform manager
     * is wrapped in a `std::unique_ptr` and returned to the caller.
     *
     * @param desiredSpecification A `PlatformSpecification` defining the desired platform type
     *                              and backend type. If either type is set to "no preference",
     *                              the function will attempt to find the best available match.
     *
     * @return A `std::unique_ptr` to the selected `PlatformManager` instance. The returned instance
     *         will reflect the best or preferred configuration available. If no suitable configuration
     *         can be found, the function may log warnings and attempt to fall back to a default or
     *         generic configuration.
     */
    export std::unique_ptr<Types::Platform::PlatformManager> selectPlatform(
        Types::Platform::PlatformManager::PlatformSpecification desiredSpecification
    );

}