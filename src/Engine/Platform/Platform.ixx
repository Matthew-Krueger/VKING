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

// horray for ifdef hell

export module VKING.Platform;

export namespace VKING::Platform {
    class Window;
    class RHI;
    class PlatformManager;

    using CreateFn = std::unique_ptr<PlatformManager> (*)();

    enum class BackendType {
        VULKAN,
        METAL,
        GNM,
        OPENGL,
        DIRECTX_12,
        BACKEND_UNSUPPORTED,
        BACKEND_NO_PREFERENCE
    };

    enum class PlatformType {
        GLFW,
        WAYLAND,
        X11,
        COCOA,
        WIN32,
        PLATFORM_UNSUPPORTED,
        PLATFORM_NO_PREFERENCE
    };

    class PlatformManager {
    public:
        struct PlatformSpecification {
            std::optional<std::unique_ptr<PlatformManager>> platform = std::nullopt;
            std::optional<CreateFn> backendCreateFN = std::nullopt;
            PlatformType platformType = PlatformType::PLATFORM_NO_PREFERENCE;
            BackendType backendType = BackendType::BACKEND_NO_PREFERENCE;
        };

        virtual ~PlatformManager() = default;

        static PlatformSpecification select(PlatformSpecification desiredSpecification);

        virtual std::unique_ptr<Window> createWindow() = 0;
        virtual std::unique_ptr<RHI> createRHI() = 0;


    private:
        PlatformManager();
    };
}

#if (VKING_HAS_GLFW == 1)
// import VKING.Platform.GLFW;
#endif

#if (VKING_HAS_VULKAN == 1)
// import VKING.Platform.Vulkan;
#endif

#if (VKING_HAS_GLFW_VULKAN_GLUE == 1)
// import VKING.Platform.Glue.GLFWVulkan;
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

namespace VKING::Platform {


    consteval auto supportedPlatforms() {
        std::vector<ScoredType<Platform::PlatformType>> platforms;

        if constexpr (VKING_HAS_GLFW == 1) {
            platforms.push_back({ .value = Platform::PlatformType::GLFW,    .score = 2 });  // cross-platform, less native
        }
        if constexpr (VKING_HAS_WAYLAND == 1) {
            platforms.push_back({ .value = Platform::PlatformType::WAYLAND, .score = 1 });  // native Linux
        }
        if constexpr (VKING_HAS_X11 == 1) {
            platforms.push_back({ .value = Platform::PlatformType::X11,     .score = 1 });  // native Linux
        }
        if constexpr (VKING_HAS_COCOA == 1) {
            platforms.push_back({ .value = Platform::PlatformType::COCOA,   .score = 1 });  // native macOS
        }
        if constexpr (VKING_HAS_WIN32 == 1) {
            platforms.push_back({ .value = Platform::PlatformType::WIN32,   .score = 1 });  // native Windows
        }

        return platforms;
    }

    consteval auto supportedBackends() {
        std::vector<ScoredType<Platform::BackendType>> backends;

        if constexpr (VKING_HAS_VULKAN == 1) {
            backends.push_back({ .value = Platform::BackendType::VULKAN,       .score = 2 });  // powerful but complex and less native
        }
        if constexpr (VKING_HAS_METAL == 1) {
            backends.push_back({ .value = Platform::BackendType::METAL,        .score = 1 });  // native Apple
        }
        if constexpr (VKING_HAS_DIRECTX_12 == 1) {
            backends.push_back({ .value = Platform::BackendType::DIRECTX_12,   .score = 1 });  // native Windows
        }
        if constexpr (VKING_HAS_OPENGL == 1) {
            backends.push_back({ .value = Platform::BackendType::OPENGL,       .score = 3 });  // legacy, avoid if possible
        }
        if constexpr (VKING_HAS_GNM == 1) {
            backends.push_back({ .value = Platform::BackendType::GNM,          .score = 1 });  // native PlayStation
        }

        return backends;
    }

    consteval uint16_t getPlatformScore(const std::vector<ScoredType<PlatformType, uint16_t>> &platforms, const PlatformType desiredPlatform) {

        auto score = std::numeric_limits<uint16_t>::max();
        uint32_t candidates = 0;
        for (size_t i = 0; i < platforms.size(); i++) {
            if (platforms[i].value == desiredPlatform) {
                score = platforms[i].score;
                candidates++;
            }
        }
        // if it is not found, return the max score
        if (candidates == 1) return score;
        return std::numeric_limits<uint16_t>::max(); // there is a duplicate somehow or the candidate does not exist this effectively removes it from consideration

    }

    consteval uint16_t getBackendScore(const std::vector<ScoredType<BackendType, uint16_t>> &backends, const BackendType desiredBackend) {

        auto score = std::numeric_limits<uint16_t>::max();
        uint32_t candidates = 0;
        for (size_t i = 0; i < backends.size(); i++) {
            if (backends[i].value == desiredBackend) {
                score = backends[i].score;
                candidates++;
            }
        }
        // if it is not found, return the max score
        if (candidates == 1) return score;
        return std::numeric_limits<uint16_t>::max(); // there is a duplicate somehow or the candidate does not exist this effectively removes it from consideration

    }

    // === Full config table with precomputed scores and factory ===
    consteval auto getConfigTable() {
        std::vector<ScoredType<PlatformManager::PlatformSpecification> > table;

#if VKING_HAS_GLFW_VULKAN_GLUE == 1
        table.push_back(
            {
                .value = {
                    .platform = std::nullopt,
                    .backendCreateFN = std::nullopt,
                    .platformType = PlatformType::GLFW,
                    .backendType = BackendType::VULKAN
                },
                .score = static_cast<uint16_t>(getPlatformScore(supportedPlatforms(), PlatformType::GLFW) * getBackendScore(supportedBackends(), BackendType::VULKAN))
            });
#endif
        // Add more as implemented...

        return table;
    }


}