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

#include "LoggerStableCABI.h"
#include <fmt/format.h>
#include <source_location>
#include <algorithm>
#include <cstdio>
#include <atomic>
#include <utility>

namespace VKING::Logger {

    using Level = VKING_Logging_Level;

    /**
     * Since Everything in detail is inline, it is installed once per binary
     * So, every plugin using the logger needs to call install with the pointer they were given
     */
    namespace detail {
        inline std::atomic<const VKING_Logging_API*> g_api{nullptr};

        /**
         * Installs the logging callback into this binary
         * @param api The API pointer to the C ABI of the engine
         */
        inline void install(const VKING_Logging_API* api) noexcept {
            constexpr uint32_t expectedABIVersion = 1;

            if (!api) {
                g_api.store(nullptr, std::memory_order_release);
                return;
            }

            if (api->abiVersion != expectedABIVersion) {
                std::fprintf(
                  stderr,
                  "Logging API ABI version mismatch: expected %u, got %u\n",
                  expectedABIVersion,
                  api->abiVersion
                );
                g_api.store(nullptr, std::memory_order_release);
                return;
            }

            // Prefer: allow newer host structs (structSize >= what we know)
            if (api->structSize < sizeof(VKING_Logging_API)) {
                std::fprintf(
                  stderr,
                  "Logging API struct too small: expected >= %zu, got %u\n",
                  sizeof(VKING_Logging_API),
                  api->structSize
                );
                g_api.store(nullptr, std::memory_order_release);
                return;
            }

            // Optional: ensure required function pointer exists
            if (!api->logMessage) {
                std::fprintf(stderr, "Logging API missing logMessage\n");
                g_api.store(nullptr, std::memory_order_release);
                return;
            }

            g_api.store(api, std::memory_order_release);
        }

        inline const VKING_Logging_API* api() noexcept {
            return g_api.load(std::memory_order_acquire);
        }
    } // namespace VKING::Logger::detail

    // New: helper to make inline string literals work as NTTP
    /**
     * @brief Helper struct to allow string literals as non-type template parameters.
     *
     * Enables compile-time logger names like Named<"Renderer"> without runtime overhead.
     *
     * @tparam N Length of the string including null terminator
     */
    template<std::size_t N>
    struct InlineString {
        /// Construct from a string literal
        constexpr InlineString(const char (&str)[N]) {
            std::copy(str, str + N, data);
        }

        char data[N]; ///< Raw character data (null-terminated)
        static constexpr std::size_t size = N - 1; ///< Length without null terminator
    };

    [[maybe_unused]] [[nodiscard]] inline Level getLevel() {

        if (!detail::api()) return VKING_LOG_OFF; // we'll lie and say its off if we don't have the api
        if (!detail::api()->getGlobalLogLevel) return VKING_LOG_OFF; // again we'll lie
        return detail::api()->getGlobalLogLevel();

    }

    [[maybe_unused]] inline void setLevel(const Level level) {

        if (!detail::api()) return; // we won't do anything
        if (!detail::api()->setGlobalLogLevel) return; // same
        detail::api()->setGlobalLogLevel(level); // and actually set the level

    }

    /**
     * @brief Compile-time named logger.
     *
     * Each instantiation with a unique name gets its own spdlog logger instance,
     * lazily created on first use. This allows per-module logging without global strings.
     *
     * Recommended usage (once per module/header):
     * @code
     * // At namespace or header scope in your module
     * using Log = VKING::Log::Named<"Renderer">;  // Replace with your module name
     * @endcode
     *
     * Then throughout the module's implementation files:
     * @code
     * void Renderer::Initialize()
     * {
     *     Log::record().info("Initializing Vulkan renderer");
     *     // ...
     *     Log::record().debug("Created swapchain with {} images", imageCount);
     * }
     *
     * void Renderer::LoadTexture(const std::string& path)
     * {
     *     if (!textureLoaded) {
     *         Log::record().warn("Failed to load texture from {}", path);
     *     } else {
     *         Log::record().trace("Successfully loaded texture {}", path);
     *     }
     * }
     *
     * void Renderer::Shutdown()
     * {
     *     Log::record().critical("Shutting down renderer subsystem");
     * }
     * @endcode
     *
     * All log messages will automatically include the correct file, line, and function information
     * and will be tagged with the module name ("Renderer" in this example).
     *
     * @tparam Name Compile-time string identifying the logger (e.g., "Renderer", "Physics")
     */
    template<InlineString Name>
    struct Named {
        /**
         * @brief Core low-level logging function.
         *
         * Forwards formatted message to the underlying spdlog logger with source location.
         *
         * @tparam Level log level
         * @tparam Args Format argument types
         * @param loc Source location (automatically captured via default argument)
         * @param fmt Format string (checked at compile time)
         * @param args Format arguments
         */
        template<VKING_Logging_Level Level, typename... Args>
        static void log(const std::source_location &loc, fmt::format_string<Args...> fmt, Args &&... args) {
            //auto logger = get(); // your Named logger
            /* logger->log(spdlog::source_loc{
                            loc.file_name(),
                            static_cast<int32_t>(loc.line()),
                            loc.function_name()
                        },
                        Level,
                        fmt,
                        std::forward<Args>(args)...
            );*/ // no longer used since it crosses the ABI
            const auto* api = detail::api();

            if (!api) {
                fprintf(stderr, "Logging API not installed in this binary. Cannot log message.\n");
                return;
            }

            if (!api->logMessage) {
                fprintf(stderr, "Logging API missing logMessage function. Cannot log message.\n");
                return;
            }

            auto msg = fmt::format(fmt, std::forward<Args>(args)...);

            api->logMessage(
              Level,
              Name.data,
              loc.file_name(),
              static_cast<int32_t>(loc.line()),
              loc.function_name(),
              msg.c_str()
            );
        }

        /**
         * @brief Helper struct providing fluent level-specific logging methods.
         *
         * Returned by record() to allow chainable calls like:
         * Named<"Audio">::record().warn("Sound {} failed to load", name);
         */
        struct LogRecorder {
            std::source_location loc; ///< Captured source location

            /// Log at trace level
            template<typename... Args>
            void trace(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_TRACE>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at debug level
            template<typename... Args>
            void debug(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_DEBUG>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at info level
            template<typename... Args>
            void info(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_INFO>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at warn level
            template<typename... Args>
            void warn(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_WARN>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at error level
            template<typename... Args>
            void error(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_ERROR>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at critical level
            template<typename... Args>
            void critical(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_CRITICAL>(loc, fmt, std::forward<Args>(args)...);
            }
        };

        /**
         * @brief Begin a log record with automatic source location capture.
         *
         * This is the primary entry point for logging in day-to-day use. It returns a temporary
         * LogRecorder object that provides fluent methods for each log level (trace, debug, info,
         * warn, error, critical).
         *
         * When used with the recommended module alias, logging becomes extremely concise:
         * @code
         * using Log = VKING::Log::Named<"Gameplay">;
         *
         * // Typical usage throughout code
         * Log::record().trace("Player position: ({}, {}, {})", x, y, z);
         * Log::record().debug("Spawning enemy at wave {}", waveNumber);
         * Log::record().info("Level '{}' loaded successfully", levelName);
         * Log::record().warn("Low FPS detected: {}", currentFPS);
         * Log::record().error("Failed to parse script file {}", scriptPath);
         * Log::record().critical("Fatal error: Out of memory during asset load");
         * @endcode
         *
         * The source location (file name, line number, and function name) is captured automatically
         * at the call site via std::source_location::current(), providing precise context in logs
         * without any extra effort from the caller.
         *
         * @param loc Defaulted to current source location
         * @return LogRecorder instance for fluent logging
         */
        static constexpr auto record(const std::source_location &loc = std::source_location::current()) {
            return LogRecorder{loc};
        }

    };
}
