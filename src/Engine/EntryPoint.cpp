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

import VKING.Application;
import VKING.Log;

/* Actual Main function */
int VKING_Main(int argc, char ** argv){

    VKING::registerLogger();
    using EntryPointLogger = VKING::Log::Named<"EntryPoint">;
    EntryPointLogger::record().info("Logger Registered. Starting VKING");

    //atexit(VKING::atExitCallback);
    EntryPointLogger::record().info("Registered AtExit callback.");

    bool shouldRestart = true;
    while (shouldRestart) {
        EntryPointLogger::record().info("Starting new application, calling VKING::createApplication()");
        VKING::Application* application = static_cast<VKING::Application*>(VKING::createApplication());
        EntryPointLogger::record().info("Application created. Calling application->run()");
        application->run();

        EntryPointLogger::record().info("Application finished. Determining if should restart.");
        shouldRestart = application->shouldRestart();
        EntryPointLogger::record().info("Should restart: {}. If true will restart after application deleted.", shouldRestart);
        EntryPointLogger::record().info("Deleting application.");
        delete application;
    }

    EntryPointLogger::record().info("Exiting, no restart requested. BYE!");

    return 0;

}
