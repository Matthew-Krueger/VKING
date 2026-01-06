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

export module VKING.Types.Platform;

export namespace VKING::Types::Platform {
    class Window;
    class RHI;
    class PlatformManager;

    using CreateFn = PlatformManager* (*)();

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

    std::string backendToString(BackendType backend) {
        switch (backend) {
            case BackendType::VULKAN: return "Vulkan";
            case BackendType::METAL: return "Metal";
            case BackendType::GNM: return "PlayStation";
            case BackendType::OPENGL: return "OpenGL";
            case BackendType::DIRECTX_12: return "DirectX 12";
            case BackendType::BACKEND_UNSUPPORTED: return "Unsupported";
            case BackendType::BACKEND_NO_PREFERENCE: return "No Preference stated";
            default: return "Unsupported";
        }
    }

    std::string platformToString(PlatformType platform) {
        switch (platform) {
            case PlatformType::GLFW: return "GLFW";
            case PlatformType::WAYLAND: return "Wayland";
            case PlatformType::X11: return "X11";
            case PlatformType::COCOA: return "Cocoa";
            case PlatformType::WIN32: return "Win32";
            case PlatformType::PLATFORM_UNSUPPORTED: return "Unsupported";
            case PlatformType::PLATFORM_NO_PREFERENCE: return "No Preference stated";
            default: return "Unsupported";
        }
    }

    class PlatformManager {
    public:
        struct PlatformSpecification {
            struct PlatformCreateInfo {
                CreateFn pfn_PlatformManagerCreate = nullptr;
            };

            //std::unique_ptr<PlatformManager>
            std::optional<PlatformCreateInfo> platformCreateInfo = std::nullopt;
            PlatformType platformType = PlatformType::PLATFORM_NO_PREFERENCE;
            BackendType backendType = BackendType::BACKEND_NO_PREFERENCE;
        };

        virtual ~PlatformManager() = default;

        //static PlatformSpecification select(PlatformSpecification desiredSpecification);

        virtual std::unique_ptr<Window> createWindow() = 0;
        virtual std::unique_ptr<RHI> createRHI() = 0;

        [[nodiscard]] PlatformType getPlatformType() const { return m_PlatformType; }
        [[nodiscard]] BackendType getBackendType() const { return m_PlatformBackendType; }
        [[nodiscard]] PlatformSpecification::PlatformCreateInfo getPlatformCreateInfo() const { return m_PlatformCreateInfo; }

    protected:
        PlatformManager(
            BackendType backendType,
            PlatformType platformType,
            PlatformSpecification::PlatformCreateInfo platformCreateInfo)
        : m_PlatformBackendType(backendType),
          m_PlatformType(platformType),
          m_PlatformCreateInfo(platformCreateInfo){};

    private:
        BackendType m_PlatformBackendType = BackendType::BACKEND_UNSUPPORTED;
        PlatformType m_PlatformType = PlatformType::PLATFORM_UNSUPPORTED;
        PlatformSpecification::PlatformCreateInfo m_PlatformCreateInfo{};

    };

}