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
/**
 * @file LoggerTemplates.hpp
 * @brief Header-only C++ logging convenience layer over the VKING logging C ABI.
 *
 * This file provides ergonomic, compile-time named loggers for engine code and
 * plugins while keeping the actual logging backend (e.g. spdlog) entirely on
 * the host side.
 *
 * Key ideas:
 * - The host exposes a C ABI function table (`VKING_Logging_API`) containing
 *   function pointers like `logMessage(...)`.
 * - Each binary (EXE or DLL) installs a pointer to that table once (typically
 *   during initialization) via `VKING::Logger::detail::install(...)`.
 * - After installation, code can log via `VKING::Logger::Named<"...">` without
 *   linking against the host logging implementation.
 *
 * ABI boundary rules:
 * - No spdlog types appear here.
 * - The only cross-binary interface is the C ABI declared in LoggerStableCABI.h.
 *
 * Threading:
 * - Installation uses an atomic pointer to ensure visibility across threads.
 * - Logging calls are safe to make concurrently as long as the host implementation
 *   of `logMessage` is thread-safe (it should be).
 *
 * Static initialization:
 * - If this header is used in a plugin, the plugin MUST NOT log before calling
 *   `detail::install(...)` (e.g., avoid logging from global constructors).
 */

#pragma once

#include "LoggerStableCABI.h"

#include <atomic>
#include <cstdio>
#include <fmt/format.h>
#include <source_location>
#include <utility>
#include <algorithm> // used for std::copy in InlineString

namespace VKING::Logger {
    /**
     * @brief Alias for the engine-defined log level enum used by the logging ABI.
     *
     * This is intentionally not tied to spdlog's level enum; the host performs any
     * necessary mapping.
     */
    using Level = VKING_Logging_Level;

    /**
     * @namespace VKING::Logger::detail
     * @brief Internal per-binary state for the header-only logger wrapper.
     *
     * Everything in this namespace is `inline`, meaning:
     * - There is one copy per binary (EXE or DLL).
     * - Each plugin DLL must install the host API table pointer for itself.
     *
     * This avoids sharing C++ globals across DLL boundaries, which is not reliable.
     */
    namespace detail {
        /**
         * @brief Pointer to the installed host logging API table for this binary.
         *
         * This is stored as an atomic pointer so that after `install()` is called,
         * later logging calls from other threads are guaranteed to observe the
         * installed pointer (acquire/release ordering).
         *
         * @note This pointer must reference host-owned storage with lifetime at least
         *       as long as this binary may log (typically process lifetime).
         */
        inline std::atomic<const VKING_Logging_API *> g_api{nullptr};

        /**
         * @brief Install the host logging API table pointer for this binary.
         *
         * This function is expected to be called:
         * - by the host during engine startup for the main executable, and
         * - by each plugin during its plugin initialization function if it will use the logging abi
         *
         * The function validates basic ABI compatibility (version/size) and that the
         * required function pointer(s) are non-null.
         *
         * @param api Pointer to a host-owned `VKING_Logging_API` table, or nullptr to
         *            uninstall/disable logging for this binary.
         *
         * @note This function is intentionally `noexcept` and uses `stderr` for
         *       diagnostics so it can be called early during startup.
         */
        inline void install(const VKING_Logging_API *api) noexcept {
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

            /**
             * Forward-compatibility rule:
             * - A newer host may append fields to VKING_Logging_API (structSize grows).
             * - An older host provides a smaller structSize.
             *
             * We require that the host struct is at least large enough to cover the
             * fields this wrapper expects to use.
             */
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

            // Required entry point
            if (!api->logMessage) {
                std::fprintf(stderr, "Logging API missing logMessage\n");
                g_api.store(nullptr, std::memory_order_release);
                return;
            }

            g_api.store(api, std::memory_order_release);
        }

        /**
         * @brief Get the installed host logging API table for this binary.
         *
         * @return Pointer to the installed `VKING_Logging_API` table, or nullptr if no
         *         table is installed.
         */
        inline const VKING_Logging_API *api() noexcept {
            return g_api.load(std::memory_order_acquire);
        }
    } // namespace detail

    /**
     * @brief Install the host logging API table pointer for this binary.
     *
     * @note Alias for `VKING::Logger::detail::install`
     *
     * This function is expected to be called:
     * - by the host during engine startup for the main executable, and
     * - by each plugin during its plugin initialization function if it will use the logging api
     *
     * The function validates basic ABI compatibility (version/size) and that the
     * required function pointer(s) are non-null.
     *
     * @param api Pointer to a host-owned `VKING_Logging_API` table, or nullptr to
     *            uninstall/disable logging for this binary.
     *
     * @note This function is intentionally `noexcept` and uses `stderr` for
     *       diagnostics so it can be called early during startup.
     */
    [[maybe_unused]] inline void install(const VKING_Logging_API *api) noexcept { detail::install(api); }

    /**
     * @brief Helper struct to allow string literals as NTTP (non-type template parameters).
     *
     * This enables usage like:
     * @code
     * using Log = VKING::Logger::Named<"Renderer">;
     * Log::record().info("Hello {}", 123);
     * @endcode
     *
     * @tparam N Length of the string literal including the null terminator.
     *
     * @note This wrapper stores the string literal bytes so `Name.data` can be used
     *       as a stable `const char*` during logging calls.
     */
    template<std::size_t N>
    struct InlineString {
        /// Construct from a string literal (copies including null terminator).
        constexpr InlineString(const char (&str)[N]) {
            std::copy(str, str + N, data);
        }

        /// Raw character data (null-terminated).
        char data[N];

        /// Length excluding null terminator.
        static constexpr std::size_t size = N - 1;
    };

    /**
     * @brief Get the host's current global log level (if available).
     *
     * @return The host's global log level, or `VKING_LOG_OFF` if no API is installed
     *         or the host did not provide `getGlobalLogLevel`.
     */
    [[maybe_unused]] [[nodiscard]] inline Level getLevel() {
        const auto *api = detail::api();
        if (!api || !api->getGlobalLogLevel) return VKING_LOG_OFF;
        return api->getGlobalLogLevel();
    }

    /**
     * @brief Set the host's global log level (if available).
     *
     * If no host API is installed or the host did not provide `setGlobalLogLevel`,
     * this function is a no-op.
     *
     * @param level Desired global log level.
     */
    [[maybe_unused]] inline void setLevel(const Level level) {
        const auto *api = detail::api();
        if (!api || !api->setGlobalLogLevel) return;
        api->setGlobalLogLevel(level);
    }

    /**
     * @brief Compile-time named logger category.
     *
     * This type does not represent an owning logger object. Instead, it provides
     * a compile-time category name and forwards formatted messages to the host via
     * the installed `VKING_Logging_API`.
     *
     * The category name is `Name.data` and is passed to the host as the `logger`
     * argument of `logMessage(...)`.
     *
     * @tparam Name Compile-time string identifying the category (e.g. "Renderer").
     */
    template<InlineString Name>
    struct Named {
        /**
         * @brief Low-level log dispatch.
         *
         * Formats the message locally using {fmt} and forwards the final string to the
         * host logger via the installed `VKING_Logging_API`.
         *
         * @tparam Level Log severity (engine ABI level).
         * @tparam Args Format argument types.
         *
         * @param loc Source location describing the call site.
         * @param fmt Format string (checked by {fmt} at compile time).
         * @param args Format arguments.
         *
         * @note This function will emit a message to stderr and return if the logging
         *       API is not installed. You may choose to change this policy (drop
         *       silently, assert in debug, etc.).
         */
        template<VKING_Logging_Level Level, typename... Args>
        static void log(
            const std::source_location &loc,
            fmt::format_string<Args...> fmt,
            Args &&... args
        ) {
            const auto *api = detail::api();

            if (!api) {
                std::fprintf(stderr, "Logging API not installed in this binary.\n");
                return;
            }

            if (!api->logMessage) {
                std::fprintf(stderr, "Logging API missing logMessage function.\n");
                return;
            }

            // Format locally in this binary; only the final message crosses the ABI.
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
         * @brief Helper object that captures source location and provides level methods.
         *
         * Returned by `record()` to support fluent usage:
         * @code
         * using Log = VKING::Logger::Named<"EngineConfig">;
         * Log::record().info("Selected backend {}", backendName);
         * @endcode
         */
        struct LogRecorder {
            /// Source location captured at the call site.
            std::source_location loc;

            /// Log at trace level.
            template<typename... Args>
            void trace(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_TRACE>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at debug level.
            template<typename... Args>
            void debug(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_DEBUG>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at info level.
            template<typename... Args>
            void info(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_INFO>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at warn level.
            template<typename... Args>
            void warn(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_WARN>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at error level.
            template<typename... Args>
            void error(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_ERROR>(loc, fmt, std::forward<Args>(args)...);
            }

            /// Log at critical level.
            template<typename... Args>
            void critical(fmt::format_string<Args...> fmt, Args &&... args) {
                log<VKING_LOG_CRITICAL>(loc, fmt, std::forward<Args>(args)...);
            }
        };

        /**
         * @brief Begin a log record with automatic source location capture.
         *
         * This is the primary entry point for day-to-day logging. It captures the
         * call site location by default and returns a lightweight recorder that offers
         * `.trace()/.debug()/.info()/.warn()/.error()/.critical()`.
         *
         * @param loc Source location; defaults to `std::source_location::current()`.
         * @return A LogRecorder containing the captured source location.
         */
        static constexpr auto record(
            const std::source_location &loc = std::source_location::current()
        ) {
            return LogRecorder{loc};
        }
    };
} // namespace VKING::Logger
