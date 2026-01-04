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
    class Log {
    public:

        using Level = spdlog::level::level_enum;

        static constexpr auto DEFAULT_LOG_PATTERN = "%^[%Y-%m-%d %H:%M:%S.%f] [%n] [%l] [%s:%#] %v %$";
        static constexpr auto DEFAULT_LOG_FILE_PATH = "VKING.log";

        // Initialize the shared sinks (call once at startup)
        static void Init(const std::string& logFilePath = DEFAULT_LOG_FILE_PATH);

        static void setFormat(const std::string& format = DEFAULT_LOG_PATTERN);

        static void setLevel(Level level);


        // New: helper to make inline string literals work as NTTP
        template <std::size_t N>
        struct InlineString {
            constexpr InlineString(const char (&str)[N]) {
                std::copy(str, str + N, data);
            }
            char data[N];
            static constexpr std::size_t size = N - 1; // without null
        };

        // The magic: a logger bound to a specific name at compile time
        template <InlineString Name>
        struct Named {
            // Core logging function — uses spdlog's macro-based stripping
            template <spdlog::level::level_enum Level, typename... Args>
            static void log(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
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

            // Convenience level functions
            template <typename... Args>
            static void trace(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::trace>(fmt, std::forward<Args>(args)..., loc);
            }

            template <typename... Args>
            static void debug(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::debug>(fmt, std::forward<Args>(args)..., loc);
            }

            template <typename... Args>
            static void info(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::info>(fmt, std::forward<Args>(args)..., loc);
            }

            template <typename... Args>
            static void warn(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::warn>(fmt, std::forward<Args>(args)..., loc);
            }

            template <typename... Args>
            static void error(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::err>(fmt, std::forward<Args>(args)..., loc);
            }

            template <typename... Args>
            static void critical(std::string_view fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
                log<spdlog::level::critical>(fmt, std::forward<Args>(args)..., loc);
            }

            // Access the underlying spdlog logger (for advanced use: flush, set_level, etc.)
            static std::shared_ptr<spdlog::logger> get();

        };
    };
}

// Private implementation

namespace VKING::detail {
    inline std::shared_ptr<spdlog::sinks::basic_file_sink_mt>   file_sink;
    inline std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
}


template <VKING::Log::InlineString Name>
std::shared_ptr<spdlog::logger> VKING::Log::Named<Name>::get() {

    if (!detail::file_sink || !detail::console_sink) {
        throw std::runtime_error("VKING::Log used before Init()!");
    }

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

void VKING::Log::Init(const std::string& logFilePath) {

    // Create the sinks. but should only happen once per engine runtime. So we'll use an atomic
    // to prevent more than one from doing it. Techncially there *may* be a race condition, but
    static std::atomic_bool s_LogInitialized{false};
    if (!s_LogInitialized.exchange(true)) {

        detail::file_sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath);
        detail::console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        spdlog::set_pattern(VKING::Log::DEFAULT_LOG_PATTERN);
        detail::file_sink->set_pattern(DEFAULT_LOG_PATTERN);
        detail::console_sink->set_pattern(DEFAULT_LOG_PATTERN);

        spdlog::flush_every(std::chrono::seconds(3));

    } else {
        throw std::runtime_error("VKING::Log::Init() called more than once!");
    }

}

void VKING::Log::setFormat(const std::string& format) {

    spdlog::set_pattern(format);
    detail::file_sink->set_pattern(format);
    detail::console_sink->set_pattern(format);

}

void VKING::Log::setLevel(VKING::Log::Level level) {
    spdlog::set_level(level);
}

// Ensure logger exists when Named<>::get() is called
