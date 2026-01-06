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
     * Retrieves a list of platform scores based on compile-time configuration constants.
     *
     * The function evaluates the available platform types based on predefined macros,
     * appending supported platforms to a vector with their corresponding scores.
     * Platform scores are assigned as follows:
     * - GLFW: Score 2 (cross-platform, less native)
     * - WAYLAND: Score 1 (native Linux)
     * - X11: Score 1 (native Linux)
     * - COCOA: Score 1 (native macOS)
     * - WIN32: Score 1 (native Windows)
     *
     * These scores can be used to prioritize platform selection in a multi-platform environment.
     *
     * @tparam VKING_HAS_GLFW Compile-time flag indicating if GLFW is available.
     * @tparam VKING_HAS_WAYLAND Compile-time flag indicating if Wayland is available.
     * @tparam VKING_HAS_X11 Compile-time flag indicating if X11 is available.
     * @tparam VKING_HAS_COCOA Compile-time flag indicating if Cocoa is available.
     * @tparam VKING_HAS_WIN32 Compile-time flag indicating if Win32 is available.
     *
     * @return A vector of `ScoredType` objects, where each object contains a supported
     *         platform type and its associated score.
     */
    constexpr auto getPlatformScores() {
        std::vector<ScoredType<Types::Platform::PlatformType>> platforms;

        if constexpr (VKING_HAS_GLFW == 1) {
            platforms.push_back({ .value = Types::Platform::PlatformType::GLFW,    .score = 2 });  // cross-platform, less native
        }
        if constexpr (VKING_HAS_WAYLAND == 1) {
            platforms.push_back({ .value = Types::Platform::PlatformType::WAYLAND, .score = 1 });  // native Linux
        }
        if constexpr (VKING_HAS_X11 == 1) {
            platforms.push_back({ .value = Types::Platform::PlatformType::X11,     .score = 1 });  // native Linux
        }
        if constexpr (VKING_HAS_COCOA == 1) {
            platforms.push_back({ .value = Types::Platform::PlatformType::COCOA,   .score = 1 });  // native macOS
        }
        if constexpr (VKING_HAS_WIN32 == 1) {
            platforms.push_back({ .value = Types::Platform::PlatformType::WIN32,   .score = 1 });  // native Windows
        }

        return platforms;
    }

    /**
     * Extracts a list of backend scores based on compile-time configuration constants.
     *
     * The function evaluates the availability of various rendering backends using predefined macros,
     * and append the supported backends along with their associated scores to the returned vector.
     * Scores are assigned to backends as follows:
     * - VULKAN: Score 2 (powerful but complex and less native)
     * - METAL: Score 1 (native Apple)
     * - DIRECTX_12: Score 1 (native Windows)
     * - OPENGL: Score 3 (legacy, avoid if possible)
     * - GNM: Score 1 (native PlayStation)
     *
     * These scores represent factors like backend performance, compatibility, and system support,
     * and are useful for prioritizing backends in a multi-backend environment.
     *
     * @tparam VKING_HAS_VULKAN Compile-time flag indicating if Vulkan is available.
     * @tparam VKING_HAS_METAL Compile-time flag indicating if Metal is available.
     * @tparam VKING_HAS_DIRECTX_12 Compile-time flag indicating if DirectX 12 is available.
     * @tparam VKING_HAS_OPENGL Compile-time flag indicating if OpenGL is available.
     * @tparam VKING_HAS_GNM Compile-time flag indicating if GNM is available.
     *
     * @return A vector of `ScoredType` objects, where each object contains a supported backend
     *         type and its associated score.
     */
    constexpr auto getBackendScores() {
        std::vector<ScoredType<Types::Platform::BackendType>> backends;

        if constexpr (VKING_HAS_VULKAN == 1) {
            backends.push_back({ .value = Types::Platform::BackendType::VULKAN,       .score = 2 });  // powerful but complex and less native
        }
        if constexpr (VKING_HAS_METAL == 1) {
            backends.push_back({ .value = Types::Platform::BackendType::METAL,        .score = 1 });  // native Apple
        }
        if constexpr (VKING_HAS_DIRECTX_12 == 1) {
            backends.push_back({ .value = Types::Platform::BackendType::DIRECTX_12,   .score = 1 });  // native Windows
        }
        if constexpr (VKING_HAS_OPENGL == 1) {
            backends.push_back({ .value = Types::Platform::BackendType::OPENGL,       .score = 3 });  // legacy, avoid if possible
        }
        if constexpr (VKING_HAS_GNM == 1) {
            backends.push_back({ .value = Types::Platform::BackendType::GNM,          .score = 1 });  // native PlayStation
        }

        return backends;
    }

    /**
     * Evaluates the score of a desired platform from a list of platform types and their associated scores.
     *
     * This function searches the provided vector of scored platform types to determine the score
     * corresponding to the specified desired platform. If the desired platform exists uniquely
     * within the list, its score is returned. If the desired platform is not found or if there
     * are multiple entries for the same platform, the function returns the maximum possible score,
     * effectively removing the platform from consideration.
     *
     * @param platforms A vector of `ScoredType` objects, where each object contains a platform type
     *                  and its associated score.
     * @param desiredPlatform The specific platform type for which the score is to be retrieved.
     *
     * @return The score of the desired platform if it is found uniquely in the list. Returns
     *         `std::numeric_limits<uint16_t>::max()` if the platform is not found or if there are
     *         duplicate entries for the platform.
     */
    constexpr uint16_t getPlatformScore(const std::vector<ScoredType<Types::Platform::PlatformType, uint16_t>> &platforms, const Types::Platform::PlatformType desiredPlatform) {

        auto score = std::numeric_limits<uint16_t>::max();
        uint32_t candidates = 0;
        for (auto [value, platformScore] : platforms) {
            if (value == desiredPlatform) {
                score = platformScore;
                candidates++;
            }
        }
        // if it is not found, return the max score
        if (candidates == 1) return score;
        return std::numeric_limits<uint16_t>::max(); // there is a duplicate somehow or the candidate does not exist this effectively removes it from consideration

    }

    /**
     * Retrieves the score of a specified backend from a list of backends with associated scores.
     *
     * The function searches through a list of backend-score pairs to find the score of a
     * specified desired backend. If the backend is found and there is exactly one match,
     * its corresponding score is returned. If the backend is not found, or if there are
     * duplicate matches, the function returns the maximum possible score, effectively
     * removing the backend from consideration.
     *
     * @param backends A vector containing pairs of backend types and their associated scores.
     * @param desiredBackend The specific backend type for which the score needs to be retrieved.
     *
     * @return The score associated with the desired backend if exactly one match is found,
     *         or the maximum possible score if no match or multiple matches are found.
     */
    constexpr uint16_t getBackendScore(const std::vector<ScoredType<Types::Platform::BackendType, uint16_t>> &backends, const Types::Platform::BackendType desiredBackend) {

        auto score = std::numeric_limits<uint16_t>::max();
        uint32_t candidates = 0;
        for (auto [value, backendScore] : backends) {
            if (value == desiredBackend) {
                score = backendScore;
                candidates++;
            }
        }
        // if it is not found, return the max score
        if (candidates == 1) return score;
        return std::numeric_limits<uint16_t>::max(); // there is a duplicate somehow or the candidate does not exist this effectively removes it from consideration

    }

    /**
     * @brief Retrieves a reference to a table of available platform configurations with precomputed scores and factory functions.
     *
     * The function computes the Cartesian product of available platforms and backends, assigning each combination a score.
     * The scores are based on individual platform and backend scoring functions. Each entry in the table contains the
     * associated platform specification and its computed score. This enables prioritization of configurations during
     * platform selection.
     *
     * A Meyers singleton is used to ensure the table is initialized only once and is shared across all invocations of the
     * function, minimizing overhead from repeated computations.
     *
     * @return A constant reference to a vector of `ScoredType` objects, where each object represents a platform specification
     *         and its corresponding score.
     */
    const std::vector<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> &getAvailablePlatformConfigurations();


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