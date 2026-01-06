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

    /**
     * @brief Enumerates the different graphics backends supported by the platform.
     *
     * This enumeration specifies the graphics backends available for use, such as Vulkan, Metal, and others.
     * It can also represent unsupported backends or the absence of a specific preference for backend selection.
     *
     * Enum values:
     * - VULKAN: Specifies the Vulkan graphics backend.
     * - METAL: Specifies the Metal graphics backend (primarily for Apple platforms).
     * - GNM: Specifies the GNM graphics backend (specific to Sony PlayStation systems).
     * - OPENGL: Specifies the OpenGL graphics backend.
     * - DIRECTX_12: Specifies the DirectX 12 graphics backend (primarily for Windows platforms).
     * - BACKEND_UNSUPPORTED: Indicates that no supported backend is available for the platform or configuration.
     * - BACKEND_NO_PREFERENCE: Used when there is no explicit preference for the graphics backend.
     */
    enum class BackendType {
        VULKAN,
        METAL,
        GNM,
        OPENGL,
        DIRECTX_12,
        BACKEND_UNSUPPORTED,
        BACKEND_NO_PREFERENCE
    };

    /**
     * @enum PlatformType
     * @brief Represents different types of supported platforms.
     *
     * This enumeration provides a list of platform types that can be targeted
     * for windowing and rendering purposes. It is used for platform-specific
     * implementations and selection in the `VKING` framework.
     *
     * The available platform types are:
     * - GLFW: Represents the GLFW cross-platform library.
     * - WAYLAND: Represents the Wayland display protocol.
     * - X11: Represents the X Window System display server.
     * - COCOA: Represents macOS's Cocoa framework.
     * - WIN32: Represents the Windows Win32 API.
     * - PLATFORM_UNSUPPORTED: Represents an unsupported platform.
     * - PLATFORM_NO_PREFERENCE: Represents a state where no specific platform preference is indicated.
     */
    enum class PlatformType {
        GLFW,
        WAYLAND,
        X11,
        COCOA,
        WIN32,
        PLATFORM_UNSUPPORTED,
        PLATFORM_NO_PREFERENCE
    };

    /**
     * Converts a BackendType enum value to its corresponding string representation.
     *
     * @param backend The BackendType enum value to be converted.
     * @return A string representation of the specified BackendType.
     *         Returns "Unsupported" if the backend type is not recognized.
     */
    constexpr std::string backendToString(BackendType backend) {
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

    /**
     * Converts a PlatformType enum value to its corresponding string representation.
     *
     * @param platform The PlatformType value to be converted.
     * @return A string representation of the specified PlatformType. Possible return values
     *         include "GLFW", "Wayland", "X11", "Cocoa", "Win32", "Unsupported", and
     *         "No Preference stated".
     */
    constexpr std::string platformToString(PlatformType platform) {
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

    /**
     * @class PlatformManager
     * @brief Abstract base class for managing platform-specific functionality and backend interfaces.
     *
     * The PlatformManager provides a unified interface for interacting with specific platforms and graphics backends.
     * It defines the structure for creating platform-dependent resources like windows and rendering hardware interfaces (RHI).
     * This class is intended to be subclassed for platform-specific implementations.
     *
     * It forms the primary interface for the "glue" that holds a windowing system (platform) and RHI (backend) together.
     */
    class PlatformManager {
        /**
         * @struct PlatformSpecification
         *
         * Represents a specification for a platform. The specification includes
         * essential configuration details for creating and managing platform-related
         * resources.
         */
    public:
        struct PlatformSpecification {
            /**
             * @struct PlatformCreateInfo
             *
             * @brief A structure that holds information necessary to create a PlatformManager instance.
             *
             * This structure is primarily used to provide a callback function responsible
             * for creating and initializing a platform-specific implementation of the
             * `PlatformManager` class.
             */
            struct PlatformCreateInfo {
                /**
                 * A function pointer of type `CreateFn` that is used to create an instance of
                 * the `PlatformManager` class. This is defined as a callable returning a pointer
                 * to a `PlatformManager` object.
                 *
                 * The purpose of `pfn_PlatformManagerCreate` is to allow the dynamic creation of
                 * platform-specific `PlatformManager` instances at runtime. The actual function
                 * assigned to this pointer may vary depending on the selected platform and backend
                 * configuration (e.g., GLFW with Vulkan, Wayland, etc.).
                 *
                 * This variable is typically set to a specific implementation function, such as
                 * `VKING_Platform_Glue_GLFWVulkan_Create`, which is responsible for instantiating
                 * a `PlatformManager` with the appropriate platform and backend details.
                 *
                 * By default, `pfn_PlatformManagerCreate` is initialized to `nullptr`, indicating
                 * that no creation function has been assigned. Users of this variable should verify
                 * that it is not null before invoking the function to avoid undefined behavior.
                 */
                CreateFn pfn_PlatformManagerCreate = nullptr;
            };

            /**
             * @brief Represents the optional configuration details for creating a platform instance.
             *
             * This variable holds an optional `PlatformCreateInfo` object, which contains the necessary
             * information to initialize and create a platform manager instance. If `std::nullopt`, no
             * configuration is provided.
             *
             * Potential use case scenarios include dynamically evaluating and selecting platform configurations
             * based on runtime requirements or preferences.
             */
            std::optional<PlatformCreateInfo> platformCreateInfo = std::nullopt;
            /**
             * @brief Specifies the platform type to be used in the platform management and selection process.
             * The value of this variable determines the preferred platform environment for the application.
             *
             * Possible values are defined by the PlatformType enum:
             * - GLFW: Indicates the platform using the GLFW library.
             * - WAYLAND: Indicates the Wayland display server protocol.
             * - X11: Indicates the X Window System.
             * - COCOA: Refers to the macOS Cocoa platform.
             * - WIN32: Indicates the Windows platform.
             * - PLATFORM_UNSUPPORTED: Represents an unsupported platform type.
             * - PLATFORM_NO_PREFERENCE: Indicates no specific platform preference.
             *
             * The default value is PLATFORM_NO_PREFERENCE, allowing the system
             * to select the most suitable platform based on the runtime environment
             * or additional specifications provided elsewhere in the configuration.
             */
            PlatformType platformType = PlatformType::PLATFORM_NO_PREFERENCE;
            /**
             * @brief Specifies the preferred backend type for rendering or computational operations.
             *
             * The `backendType` variable determines the type of backend that should be used for the platform.
             * It can hold various values representing different graphics or compute backends, such as Vulkan,
             * Metal, OpenGL, DirectX, or GNM. It also includes options for unsupported backends and for cases
             * where no specific backend preference is indicated.
             *
             * By default, this variable is initialized to `BackendType::BACKEND_NO_PREFERENCE`, indicating
             * that there is no specific preference for the backend type and the system can select an appropriate
             * one based on availability and compatibility.
             *
             * Possible enumerator values include:
             * - `BackendType::VULKAN`: Represents Vulkan backend.
             * - `BackendType::METAL`: Represents Metal backend.
             * - `BackendType::GNM`: Represents GNM backend.
             * - `BackendType::OPENGL`: Represents OpenGL backend.
             * - `BackendType::DIRECTX_12`: Represents DirectX 12 backend.
             * - `BackendType::BACKEND_UNSUPPORTED`: Represents an unsupported backend.
             * - `BackendType::BACKEND_NO_PREFERENCE`: Indicates no specific backend preference.
             */
            BackendType backendType = BackendType::BACKEND_NO_PREFERENCE;
        };

        /**
         * @brief Virtual destructor for the PlatformManager class.
         *
         * This method ensures that derived classes can correctly release their resources
         * when a PlatformManager object is deleted. It is declared as `virtual` and set
         * to `default`.
         */
        virtual ~PlatformManager() = default;

        /**
         * @brief Creates a generic window (backed by a platform specific window)
         *
         * Creates and returns a unique pointer to a Window instance.
         * The derived class implements the specific logic for creating
         * a platform-specific or backend-specific window.
         *
         * @return A unique pointer to the created Window instance.
         */
        virtual std::unique_ptr<Window> createWindow() = 0;

        /**
         * @brief Creates a generic RHI (backed by a platform specific RHI)
         *
         * Creates a new instance of the RHI (Rendering Hardware Interface) specific to the platform and backend.
         * This method must be implemented by derived classes of PlatformManager
         * to provide the appropriate RHI implementation.
         *
         * @return A unique_ptr to the newly created RHI instance.
         */
        virtual std::unique_ptr<RHI> createRHI() = 0;

        /**
         * @brief Retrieves the type of platform currently being used by the platform manager.
         *
         * @return The platform type as a value of the PlatformType enumeration.
         */
        [[nodiscard]] PlatformType getPlatformType() const { return m_PlatformType; }
        /**
         * @brief Retrieves the backend type currently used by the platform manager.
         *
         * @return The backend type as an enum value of type BackendType.
         *         Possible values include VULKAN, METAL, GNM, OPENGL, DIRECTX_12,
         *         BACKEND_UNSUPPORTED, and BACKEND_NO_PREFERENCE.
         */
        [[nodiscard]] BackendType getBackendType() const { return m_PlatformBackendType; }
        /**
         * Retrieves the platform creation information for the current platform manager instance.
         * The returned structure contains details and function pointers needed for platform-specific
         * initialization.
         *
         * This generally does not need to be called and is invoked automatically. It is stored "just in case"
         *
         * @return A PlatformSpecification::PlatformCreateInfo struct containing platform-specific
         *         creation information.
         */
        [[nodiscard]] PlatformSpecification::PlatformCreateInfo getPlatformCreateInfo() const { return m_PlatformCreateInfo; }


    protected:
        /**
         * Constructs a PlatformManager instance, initializing it with the specified backend type,
         * platform type, and platform creation info.
         *
         * @param backendType The type of rendering backend to use, represented by the BackendType enum.
         * @param platformType The target platform for which the manager is created, represented by the PlatformType enum.
         * @param platformCreateInfo The structure containing platform-specific creation details and a function pointer for creating the manager.
         * @return A newly constructed instance of PlatformManager.
         */
        PlatformManager(
            BackendType backendType,
            PlatformType platformType,
            PlatformSpecification::PlatformCreateInfo platformCreateInfo)
        : m_PlatformBackendType(backendType),
          m_PlatformType(platformType),
          m_PlatformCreateInfo(platformCreateInfo){};

        /**
         * Represents the backend type currently configured for the platform.
         *
         * This variable holds the specific rendering or graphics backend type
         * being utilized by the platform. The available backend types are
         * defined in the `BackendType` enum, which includes supported backends
         * like Vulkan, Metal, OpenGL, DirectX 12, and others. By default, the
         * value is set to `BackendType::BACKEND_UNSUPPORTED`, indicating that
         * no valid backend has been selected or supported on the current platform.
         *
         * Users may refer to this variable to query the backend type being actively
         * used or configured by the platform system.
         */
    private:
        BackendType m_PlatformBackendType = BackendType::BACKEND_UNSUPPORTED;
        /**
         * @brief Specifies the type of platform currently in use.
         *
         * This variable is initialized to `PlatformType::PLATFORM_UNSUPPORTED` by default,
         * indicating that the platform type is not supported or not specified.
         * Possible values are defined in the `PlatformType` enumeration, which includes
         * options such as `GLFW`, `WAYLAND`, `X11`, `COCOA`, `WIN32`, among others.
         *
         * The platform type is used to identify the specific platform being targeted
         * for windowing and rendering operations, and influences platform-specific
         * behavior in the application.
         *
         * @see VKING::Types::Platform::PlatformType
         */
        PlatformType m_PlatformType = PlatformType::PLATFORM_UNSUPPORTED;
        /**
         * Holds platform-specific creation information required for initializing a
         * platform manager. This structure is part of the
         * VKING::Types::Platform::PlatformManager::PlatformSpecification.
         *
         * It contains function pointers or handles to aid the creation of platform
         * managers as per the desired platform and backend requirements.
         */
        PlatformSpecification::PlatformCreateInfo m_PlatformCreateInfo{};

    };

}