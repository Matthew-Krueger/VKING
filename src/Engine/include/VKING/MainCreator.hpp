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

#pragma once

#include <VKING/Prerequisites.hpp>

extern int VKING_Main([[maybe_unused]] int argc, [[maybe_unused]] const char ** _argv);

#ifdef VKING_INCLUDE_WIN_MAIN
#   ifdef WIN32
#       ifdef APIENTRY
#           undef APIENTRY
#       endif
#   include <windows.h>
    INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPWSTR, INT)
    {

        INT returnCode;
        UNREFERENCED_PARAMETER(hInst);
        UNREFERENCED_PARAMETER(hPrevInstance);

        int argc;
        char** argv;
        {
            LPWSTR* lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );
            argv = (char**) malloc( argc*sizeof(char*) );
            int size, i = 0;
            for( ; i < argc; ++i ) {
                size = wcslen( lpArgv[i] ) + 1;
                argv[i] = (char*) malloc( size );
                wcstombs( argv[i], lpArgv[i], size );
            }

            returnCode = (INT) VKING_Main(argc, argv);

            for(int i = 0; i < argc; ++i) free(argv[i]);
            free(argv);
            LocalFree(lpArgv);
        }

        return returnCode;

    }

#   endif

#endif


#ifndef VKING_SUPPRESS_ENTRY_POINT_MESSAGES
#pragma message("==================================================================")
#pragma message("VKING Engine Entry Point Guidelines:")
#pragma message("")
#pragma message("  The engine provides a controlled entry point via VKING_Main(int argc, char** argv).")
#pragma message("  This function MUST be called exactly once in your binary.")
#pragma message("")
#pragma message("  Options:")
#pragma message("    • Define VKING_INCLUDE_POSIX_MAIN  to let the engine provide int main()")
#pragma message("    • Define VKING_INCLUDE_WIN_MAIN     to let the engine provide wWinMain() on Windows")
#pragma message("    • Or write your own main()/wWinMain() and explicitly call VKING_Main(argc, argv)")
#pragma message("")
#pragma message("  Custom initialization (e.g. logger setup) should be done by implementing:")
#pragma message("      extern void VKING::registerLogger();")
#pragma message("  This is called at the very start of VKING_Main, before createApplication().")
#pragma message("")
#pragma message("  To suppress these messages (after you have read and understood them),")
#pragma message("  define VKING_SUPPRESS_ENTRY_POINT_MESSAGES before including this header.")
#pragma message("==================================================================")
#endif

#ifdef VKING_INCLUDE_POSIX_MAIN
// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main(int argc, char ** argv) {
    return VKING_Main(argc, const_cast<const char**>(argv));
}
#endif


