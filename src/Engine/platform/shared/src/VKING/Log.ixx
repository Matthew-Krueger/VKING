/*
 *         A high-performance, module-first game engine.
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
// Created by Matthew Krueger on 1/3/26.
//
// VKING.Log.ixx (module interface)
module;

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

export module VKING.Log;


export namespace VKING {
    /**
     * @brief Central logging class.
     *
     * All logging functionality is accessed through this class. It manages global initialization
     * and provides access to per-module named loggers via the Named<> template.
     *
     * The intended usage pattern is to create a type alias in each module or subsystem header:
     * @code
     * // In Renderer.hpp (or any module header)
     * using Log = VKING::Log::Named<"Renderer">;
     *
     * // In Physics.hpp
     * using Log = VKING::Log::Named<"Physics">;
     *
     * // In Audio.hpp
     * using Log = VKING::Log::Named<"Audio">;
     * @endcode
     *
     * This alias allows clean, per-module logging throughout the codebase without repeating
     * the template syntax. After the alias is in place, logging is done via:
     * @code
     * Log::record().info("Initialized Vulkan context");
     * Log::record().warn("Failed to load texture {}", path);
     * Log::record().error("Render pass creation failed: {}", errorMsg);
     * @endcode
     *
     * The source file, line number, and function name are automatically captured.
     */
    class Log {
    public:
        /// Alias for spdlog log levels (trace, debug, info, warn, err, critical)
        using Level = spdlog::level::level_enum;

        /// Default log pattern used for both console and file output
        static constexpr auto DEFAULT_LOG_PATTERN = "%^[%Y-%m-%d %H:%M:%S.%f] [%n] [%l] [%s:%#] %v %$";
        /// Default file path for log output
        static constexpr auto DEFAULT_LOG_FILE_PATH = "VKING.log";

        /**
         * @brief Initialize the logging system.
         *
         * Must be called exactly once at engine startup before any logging occurs.
         * Creates shared file and console sinks and registers them with spdlog.
         *
         * @param logFilePath Path to the log file (defaults to "VKING.log")
         */
        static void Init(const std::string& logFilePath = DEFAULT_LOG_FILE_PATH);

        /**
         * @brief Set the log pattern for all sinks.
         *
         * @param format Custom pattern (defaults to DEFAULT_LOG_PATTERN if empty)
         */
        static void setFormat(const std::string& format = DEFAULT_LOG_PATTERN);

        /**
         * @brief Set the global minimum log level.
         *
         * Messages below this level will be dropped.
         *
         * @param level Desired log level
         */
        static void setLevel(Level level);


        // New: helper to make inline string literals work as NTTP
        /**
         * @brief Helper struct to allow string literals as non-type template parameters.
         *
         * Enables compile-time logger names like Named<"Renderer"> without runtime overhead.
         *
         * @tparam N Length of the string including null terminator
         */
        template <std::size_t N>
        struct InlineString {
            /// Construct from a string literal
            constexpr InlineString(const char (&str)[N]) {
                std::copy(str, str + N, data);
            }
            char data[N];                         ///< Raw character data (null-terminated)
            static constexpr std::size_t size = N - 1; ///< Length without null terminator
        };

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
        template <InlineString Name>
        struct Named {
            /**
             * @brief Core low-level logging function.
             *
             * Forwards formatted message to the underlying spdlog logger with source location.
             *
             * @tparam Level spdlog log level
             * @tparam Args Format argument types
             * @param loc Source location (automatically captured via default argument)
             * @param fmt Format string (checked at compile time)
             * @param args Format arguments
             */
            template <spdlog::level::level_enum Level, typename... Args>
            static void log(const std::source_location& loc, fmt::format_string<Args...> fmt, Args&&... args) {
                auto logger = get();  // your Named logger
                logger->log(spdlog::source_loc{
                        loc.file_name(),
                        static_cast<int32_t>(loc.line()),
                        loc.function_name()
                    },
                    Level,
                    fmt,
                    std::forward<Args>(args)...
                );
            }

            /**
             * @brief Helper struct providing fluent level-specific logging methods.
             *
             * Returned by record() to allow chainable calls like:
             * Named<"Audio">::record().warn("Sound {} failed to load", name);
             */
            struct LowWriter {
                std::source_location loc;  ///< Captured source location

                /// Log at trace level
                template <typename... Args>
                void trace(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::trace>(loc, fmt, std::forward<Args>(args)...);
                }

                /// Log at debug level
                template <typename... Args>
                void debug(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::debug>(loc, fmt, std::forward<Args>(args)...);
                }

                /// Log at info level
                template <typename... Args>
                void info(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::info>(loc, fmt, std::forward<Args>(args)...);
                }

                /// Log at warn level
                template <typename... Args>
                void warn(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::warn>(loc, fmt, std::forward<Args>(args)...);
                }

                /// Log at error level
                template <typename... Args>
                void error(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::err>(loc, fmt, std::forward<Args>(args)...);
                }

                /// Log at critical level
                template <typename... Args>
                void critical(fmt::format_string<Args...> fmt, Args&&... args) {
                    log<spdlog::level::critical>(loc, fmt, std::forward<Args>(args)...);
                }
            };

            /**
             * @brief Begin a log record with automatic source location capture.
             *
             * This is the primary entry point for logging in day-to-day use. It returns a temporary
             * LowWriter object that provides fluent methods for each log level (trace, debug, info,
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
             * @return LowWriter instance for fluent logging
             */
            static constexpr auto record(const std::source_location& loc = std::source_location::current()) {
                return LowWriter{loc};
            }

            /**
             * @brief Access the underlying spdlog logger.
             *
             * Useful for advanced operations like flushing or per-logger level changes.
             *
             * @return Shared pointer to the spdlog logger
             * @throws std::runtime_error if called before Log::Init()
             */
            static std::shared_ptr<spdlog::logger> get();

        };
    };
}

// Private implementation details (not exported)

// Namespace containing global sink pointers shared across all loggers
namespace VKING::detail {
    inline std::shared_ptr<spdlog::sinks::basic_file_sink_mt>   file_sink;     // File sink (thread-safe)
    inline std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;  // Colored console sink (thread-safe)
}

// Template definition: retrieve or create the named spdlog logger
template <VKING::Log::InlineString Name>
std::shared_ptr<spdlog::logger> VKING::Log::Named<Name>::get() {

    if (!detail::file_sink || !detail::console_sink) {
        throw std::runtime_error("VKING::Log used before Init()!");
    }

    // Static local ensures thread-safe lazy initialization (Meyers' singleton)
    static std::shared_ptr<spdlog::logger> s_Logger = [] {
        auto logger = spdlog::get(Name.data);
        if (!logger) {
            std::vector<spdlog::sink_ptr> sinks{ detail::file_sink, detail::console_sink };
            logger = std::make_shared<spdlog::logger>(Name.data, sinks.begin(), sinks.end());
            spdlog::register_logger(logger);
        }
        return logger;
    }();
    return s_Logger;

}

// Initialize global sinks (called once)
void VKING::Log::Init(const std::string& logFilePath) {

    // Ensure initialization happens only once
    static std::atomic_bool s_LogInitialized{false};
    if (!s_LogInitialized.exchange(true)) {

        detail::file_sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath);
        detail::console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        spdlog::set_pattern(VKING::Log::DEFAULT_LOG_PATTERN);
        detail::file_sink->set_pattern(DEFAULT_LOG_PATTERN);
        detail::console_sink->set_pattern(DEFAULT_LOG_PATTERN);

        // Automatic flush every 3 seconds to reduce data loss on crash
        spdlog::flush_every(std::chrono::seconds(3));

    } else {
        std::cerr<< "VKING::Log::Init() called more than once!" << std::endl;
    }

}

// Apply new pattern to all sinks
void VKING::Log::setFormat(const std::string& format) {

    spdlog::set_pattern(format);
    detail::file_sink->set_pattern(format);
    detail::console_sink->set_pattern(format);

}

// Apply new global log level
void VKING::Log::setLevel(VKING::Log::Level level) {
    spdlog::set_level(level);
}

// Ensure logger exists when Named<>::get() is called