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

module VKING.EngineConfig;
import VKING.Types.Platform;


#if (VKING_HAS_GLFW == 1)
// import VKING.Platform.GLFW;
#endif

#if (VKING_HAS_VULKAN == 1)
// import VKING.Platform.Vulkan;
#endif

#if (VKING_HAS_GLFW_VULKAN_GLUE == 1)
import VKING.Platform.Glue.GLFWVulkan;
//extern "C" VKING::Platform::PlatformManager* VKING_Platform_Glue_GLFWVulkan_Create();
#endif

#if (VKING_HAS_AT_LEAST_ONE_PLATFORM == 0)
#error "VKING Engine cannot be built: At least one platform (e.g., GLFW) must be enabled."
#endif

#if (VKING_HAS_AT_LEAST_ONE_BACKEND == 0)
#error "VKING Engine cannot be built: At least one backend (e.g., Vulkan) must be enabled."
#endif

#if (VKING_HAS_AT_LEAST_ONE_PLATFORM_BACKEND_GLUE == 0)
#error "VKING Engine cannot be built: At least one platform-backend glue (e.g., GLFW+Vulkan) must be enabled."
#endif

import VKING.Log;

namespace VKING::EngineConfig {

    using EngineConfigurationLogger = Log::Named<"EngineConfig">;

    // === Full config table with precomputed scores and factory ===
    const std::vector<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> &getAvailablePlatformConfigurations() {

        // meyers singleton since the cartesian product of available products may become large
        // and the function is not constexpr-able due to the need for link time addresses
        static std::vector<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> s_Table = [] {
            std::vector<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> table;

#if VKING_HAS_GLFW_VULKAN_GLUE == 1
            table.push_back(
                {
                    .value = {
                        .platformCreateInfo = std::make_optional(Types::Platform::PlatformManager::PlatformSpecification::PlatformCreateInfo{
                            .pfn_PlatformManagerCreate = VKING_Platform_Glue_GLFWVulkan_Create
                        }),
                        .platformType = Types::Platform::PlatformType::GLFW,
                        .backendType = Types::Platform::BackendType::VULKAN
                    },
                    .score = static_cast<uint16_t>(getPlatformScore(supportedPlatforms(), Types::Platform::PlatformType::GLFW) * getBackendScore(supportedBackends(), Types::Platform::BackendType::VULKAN))
                });
#endif
            // Add more as implemented...

            return table;
        }();

        return s_Table;

    }

    std::unique_ptr<Types::Platform::PlatformManager> selectPlatform(
        Types::Platform::PlatformManager::PlatformSpecification desiredSpecification
    ) {

        EngineConfigurationLogger::record().info("Attempting to select platform: {}", platformToString(desiredSpecification.platformType));
        EngineConfigurationLogger::record().info("Attempting to select backend: {}", backendToString(desiredSpecification.backendType));

        // first, let's get the config table
        const auto &table = getAvailablePlatformConfigurations();

        // --- Step 1: Try to find an exact match first ---
        std::optional<ScoredType<Types::Platform::PlatformManager::PlatformSpecification>> preferredCandidate;
        uint16_t minPreferredScore = std::numeric_limits<uint16_t>::max();

        for (const auto& entry : table) {
            const bool platformMatches = (desiredSpecification.platformType == Types::Platform::PlatformType::PLATFORM_NO_PREFERENCE ||
                                    desiredSpecification.platformType == entry.value.platformType);
            const bool backendMatches = (desiredSpecification.backendType == Types::Platform::BackendType::BACKEND_NO_PREFERENCE ||
                                   desiredSpecification.backendType == entry.value.backendType);

            if (platformMatches && backendMatches) {
                if (entry.score < minPreferredScore) { // Find the best-scoring among exact matches
                    minPreferredScore = entry.score;
                    preferredCandidate = entry;
                }
            }
        }

        if (preferredCandidate && preferredCandidate->score != std::numeric_limits<uint16_t>::max()) {
            EngineConfigurationLogger::record().debug("Found suitable requested backend configuration with score {}. (lower is better)", preferredCandidate->score);
            // Proceed with this candidate
        } else {

            EngineConfigurationLogger::record().warn("Requested backend configuration is unsuitable or not found. Attempting to find the best available match.");
            preferredCandidate = std::nullopt; // Reset to find best overall

            minPreferredScore = std::numeric_limits<uint16_t>::max();
            for (const auto& entry : table) {
                if (entry.score < minPreferredScore && entry.score != std::numeric_limits<uint16_t>::max()) {
                    minPreferredScore = entry.score;
                    preferredCandidate = entry;
                }
            }

            if (!preferredCandidate) {
                EngineConfigurationLogger::record().critical("No suitable platform/backend configuration found in the table.");
                return {}; // Return an empty/uninitialized spec indicating failure
            }
            EngineConfigurationLogger::record().info("Falling back to best available configuration with score {}. (lower is better)", preferredCandidate->score);

        }

        EngineConfigurationLogger::record().info("Selected platform: {}", Types::Platform::platformToString(preferredCandidate->value.platformType));
        EngineConfigurationLogger::record().info("Selected backend: {}", Types::Platform::backendToString(preferredCandidate->value.backendType));


        // at this point, preferredCandidate now has our best or preferred candidate
        // now, we call its function pointer, wrap the result, and bam, we're done

        Types::Platform::PlatformManager* result = nullptr;

        if (preferredCandidate->value.platformCreateInfo.has_value()) {
            result = preferredCandidate->value.platformCreateInfo->pfn_PlatformManagerCreate();
        }else {
            EngineConfigurationLogger::record().critical("Platform creation function not found in the previously mentioned best viable candidate.");
        }

        if (!result) {
            EngineConfigurationLogger::record().critical("Platform creation function returned nullptr.");
            return nullptr;
        }

        // result successful
        return std::unique_ptr<Types::Platform::PlatformManager>(result);

    }


}