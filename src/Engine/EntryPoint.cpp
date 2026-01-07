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
// Created by Matthew Krueger on 1/4/26.
//

#define VKING_SUPPRESS_ENTRY_POINT_MESSAGES
#include <VKING/MainCreator.hpp>
#include <VKING/Engine/Shutdown.hpp>
#include <VKING/Logger.hpp>
#include "Logging/LoggerHost.hpp"

import VKING.Application;
import VKING.EntryPointCallbacks;

static constexpr VKING::Logger::Level VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL = VKING::Logger::Level::VKING_LOG_TRACE;

/* Actual Main function */
int VKING_Main([[maybe_unused]] int argc, [[maybe_unused]] const char ** _argv){

    VKING::Logger::Host::Init(VKING::getLoggerConfig());

    // no matter what we will override the logger level here
    // save what the consumer had set
    // and put it back right before we start
    VKING::Logger::Level previousLevel = VKING::Logger::getLevel();
    VKING::Logger::setLevel(VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL);

    using EntryPointLogger = VKING::Logger::Named<"EntryPoint">;
    EntryPointLogger::record().info("Global Logger Sinks Registered via VKING::RegisterLogger() callback. Starting VKING");

    VKING::Shutdown::registerInterruptHandler();
    EntryPointLogger::record().info("Interrupt handler registered.");

    //atexit(VKING::atExitCallback);
    EntryPointLogger::record().info("Registered AtExit callback.");

    // now put it back (before we enter the loop, so we can reuse the variable
    // otherwise there's some weird issues, so we're doing the level dance and pulling it right back out
    // but it must be read in the loop, so we can reuse the variable
    VKING::Logger::setLevel(previousLevel);

    bool shouldRestart = true;
    uint32_t restartCount = 0;
    while (shouldRestart) {
        // no matter what we will override the logger level here
        // save what the consumer had set
        // and put it back right before we start
        previousLevel = VKING::Logger::getLevel();
        VKING::Logger::setLevel(VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL);

        // print a helpful message about whether or not this is the first start
        // in this invocation
        EntryPointLogger::record().info("Starting new application, restart count: {}", restartCount++);

        // other per instance initialization stuff not controlled by user
        // think globals, singletons, etc that the engine fully controls and is not tied to user's application lifetime
        // there are none right now

        // start the application respecting consumer's log level
        EntryPointLogger::record().info("Starting new application, calling VKING::createApplication(), respecting consumer log level");
        VKING::Logger::setLevel(previousLevel);
        auto application = VKING::createApplication();

        // run the event loop
        EntryPointLogger::record().info("Application created. Calling application->run()");
        application->run();

        // no matter what we will override the logger level here
        // save what the consumer had set
        // and put it back right before we start
        previousLevel = VKING::Logger::getLevel();
        VKING::Logger::setLevel(VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL);

        // Check the shutdown condition and whether or not we should restart
        EntryPointLogger::record().info("Application finished running. Checking for shutdown condition.");
        const VKING::Shutdown::Info shutdownInfo = VKING::Shutdown::getReason();
        shouldRestart = VKING::Shutdown::restartRequested();
        EntryPointLogger::record().info("Application will {} restart.", shouldRestart ? "now" : "not");

        // print why we shut down
        EntryPointLogger::record().info("Application shutdown reason: {}", VKING::Shutdown::reasonToString(shutdownInfo.reason));
        if (shutdownInfo.reason != VKING::Shutdown::Reason::REASON_NONE && !shutdownInfo.message.empty()) {
            EntryPointLogger::record().info("Shutdown message: {}", shutdownInfo.message);
        }


        // call the destructor, respecting the consumer's log level choice
        EntryPointLogger::record().info("Deleting application.");
        // once more with the log level dance
        VKING::Logger::setLevel(previousLevel);
        application.reset(); // to control the lifetime and invalidate this pointer.

        previousLevel = VKING::Logger::getLevel();
        VKING::Logger::setLevel(VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL);

        EntryPointLogger::record().info("Application deleted.");

        // and clear the shutdown request just in case, no log needed it will only confuse readers of the log
        VKING::Shutdown::clearRequest();

        VKING::Logger::setLevel(previousLevel);

    }

    // one last message at *our* controlled log level, doing the dance one last time
    // putting it back for global destructors
    previousLevel = VKING::Logger::getLevel();
    VKING::Logger::setLevel(VKING_CONTROLLED_LIFECYCLE_LOG_LEVEL);
    EntryPointLogger::record().info("Exiting, no restart requested. BYE!");
    VKING::Logger::setLevel(previousLevel);

    return 0;

}
