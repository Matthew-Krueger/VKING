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
#include "../../SDK/ABI/HostLoggingABISpec.h"
#include "../../SDK/Logger/LoggerTemplates.hpp"
#include "LoggerHost.hpp"
#include "../../SDK/ABI/VKINGHostToPluginABI.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <atomic>



extern "C" VKING_CALL void VKING_Logging_LogMessage(const VKING_Logging_Level level, const char *logger, const char *file,
                                                        const int32_t line, const char *function, const char *message);
extern "C" VKING_CALL VKING_Logging_Level VKING_Logging_getGlobalLogLevel();
extern "C" VKING_CALL void VKING_Logging_setGlobalLogLevel(VKING_Logging_Level level);


namespace VKING::Logger {
    namespace detail {
        static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> g_FileSink;
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> g_ConsoleSink;
        static std::shared_ptr<spdlog::logger> g_Logger;
    }

    spdlog::level::level_enum getLogLevelFromVKING_Logging_Level(const VKING_Logging_Level level);
    VKING_Logging_Level getLogLevelFromSpdlogLevel(const spdlog::level::level_enum level);


    const VKING_Hostside_Logging_API * Host::hostsideGetLoggingAPISpec() {

        static VKING_Hostside_Logging_API api ={
            .abiVersion = 1,
            .structSize = sizeof(VKING_Hostside_Logging_API),
            .logMessage = VKING_Logging_LogMessage,
            .getGlobalLogLevel = VKING_Logging_getGlobalLogLevel,
            .setGlobalLogLevel = VKING_Logging_setGlobalLogLevel
        };

        return &api;

    }

}


extern "C" VKING_CALL void VKING_Logging_LogMessage(const VKING_Logging_Level level, const char *logger, const char *file,
                                                    const int32_t line, const char *function, const char *message) {

    VKING::Logger::detail::g_Logger->log(
        spdlog::source_loc{file, line, function},
        VKING::Logger::getLogLevelFromVKING_Logging_Level(level),
        "[{}] {}",
        logger,
        message
    );

}

extern "C" VKING_CALL VKING_Logging_Level VKING_Logging_getGlobalLogLevel() {
    return VKING::Logger::getLogLevelFromSpdlogLevel(spdlog::get_level());
}

extern "C" VKING_CALL void VKING_Logging_setGlobalLogLevel(VKING_Logging_Level level) {

    const auto level2 = VKING::Logger::getLogLevelFromVKING_Logging_Level(level);

    spdlog::set_level(level2);
    VKING::Logger::detail::g_ConsoleSink->set_level(level2);
    VKING::Logger::detail::g_FileSink->set_level(level2);

    spdlog::apply_all([&](const std::shared_ptr<spdlog::logger>& logger) {
        logger->set_level(level2);
    });

}

namespace VKING::Logger {

    spdlog::level::level_enum getLogLevelFromVKING_Logging_Level(const VKING_Logging_Level level) {
        switch (level) {
            case VKING_LOG_TRACE: return spdlog::level::trace;
            case VKING_LOG_DEBUG: return spdlog::level::debug;
            case VKING_LOG_INFO: return spdlog::level::info;
            case VKING_LOG_WARN: return spdlog::level::warn;
            case VKING_LOG_ERROR: return spdlog::level::err;
            case VKING_LOG_CRITICAL: return spdlog::level::critical;
            default: return spdlog::level::off;
        }
    }

    VKING_Logging_Level getLogLevelFromSpdlogLevel(const spdlog::level::level_enum level) {
        switch (level) {
            case spdlog::level::trace: return VKING_LOG_TRACE;
            case spdlog::level::debug: return VKING_LOG_DEBUG;
            case spdlog::level::info: return VKING_LOG_INFO;
            case spdlog::level::warn: return VKING_LOG_WARN;
            case spdlog::level::err: return VKING_LOG_ERROR;
            case spdlog::level::critical: return VKING_LOG_CRITICAL;
            default: return VKING_LOG_OFF;
        }
    }

    namespace Host {
        void Init(CreateInfo loggerCreateInfo) {
            // Ensure initialization happens only once
            static std::atomic_bool s_LogInitialized{false};
            if (!s_LogInitialized.exchange(true)) {

                detail::install(hostsideGetLoggingAPISpec());

                spdlog::set_level(getLogLevelFromVKING_Logging_Level(loggerCreateInfo.level));

                detail::g_FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(loggerCreateInfo.fileName);
                detail::g_ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

                spdlog::set_pattern(Host::DEFAULT_LOG_PATTERN);
                detail::g_ConsoleSink->set_pattern(Host::DEFAULT_LOG_PATTERN);
                detail::g_FileSink->set_pattern(Host::DEFAULT_LOG_PATTERN);
                detail::g_ConsoleSink->set_level(getLogLevelFromVKING_Logging_Level(loggerCreateInfo.level));
                detail::g_FileSink->set_level(getLogLevelFromVKING_Logging_Level(loggerCreateInfo.level));

                // Automatic flush every 3 seconds to reduce data loss on crash
                spdlog::flush_every(std::chrono::seconds(3));

                // lastly, we'll aquire one logger
                detail::g_Logger = spdlog::get("VKING");
                if (!detail::g_Logger) {
                    std::vector<spdlog::sink_ptr> sinks{detail::g_FileSink, detail::g_ConsoleSink};
                    detail::g_Logger = std::make_shared<spdlog::logger>("VKING", sinks.begin(), sinks.end());
                    detail::g_Logger->set_level(getLogLevelFromVKING_Logging_Level(loggerCreateInfo.level));
                    spdlog::register_logger(detail::g_Logger);
                    const auto loc = std::source_location::current();
                    detail::g_Logger->log(spdlog::source_loc{
                                    loc.file_name(),
                                    static_cast<int32_t>(loc.line()),
                                    loc.function_name()
                                },
                                spdlog::level::trace,
                                "Logger VKING did not exist within spdlog and was initialized and registered."
                    );
                } else {
                    // External logger exists — be cooperative but cautious

                    // Ensure our sinks are present (idempotently)
                    auto &existing_sinks = detail::g_Logger->sinks();
                    bool has_file = false, has_console = false;
                    for (const auto &sink: existing_sinks) {
                        if (sink == detail::g_FileSink) has_file = true;
                        if (sink == detail::g_ConsoleSink) has_console = true;
                    }

                    if (!has_file) existing_sinks.push_back(detail::g_FileSink);
                    if (!has_console) existing_sinks.push_back(detail::g_ConsoleSink);

                    // level to match VKING globals
                    //logger->set_pattern(spdlog::get_pattern());
                    detail::g_Logger->set_pattern(Host::DEFAULT_LOG_PATTERN);
                    // for now we'll just set the pattern to match our constexpr default, this might break some things, but if the consumer gets here, they chose the footgun, not me.
                    detail::g_Logger->set_level(getLogLevelFromVKING_Logging_Level(loggerCreateInfo.level));

                    const auto loc = std::source_location::current();
                    detail::g_Logger->log(spdlog::source_loc{
                                    loc.file_name(),
                                    static_cast<int32_t>(loc.line()),
                                    loc.function_name()
                                },
                                spdlog::level::err,
                                "Logger VKING already exists, but VKING is trying to register it for the first time. Adding VKING sinks. Please reconsider and see if you can avoid registering the logger yourself."
                    );
                }
            } else {
                std::cerr << "VKING::Log::Init() called more than once!" << std::endl;
            }
        }
    }

}

