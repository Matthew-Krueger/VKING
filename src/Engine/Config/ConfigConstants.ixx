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

        constexpr auto supportedPlatforms() {
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

    constexpr auto supportedBackends() {
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

    const std::vector<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> &getAvailablePlatformConfigurations();

    export std::unique_ptr<Types::Platform::PlatformManager> selectPlatform(
        Types::Platform::PlatformManager::PlatformSpecification desiredSpecification
    );

}