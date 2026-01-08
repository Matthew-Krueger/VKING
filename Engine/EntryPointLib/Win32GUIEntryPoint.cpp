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
// Created by Matthew Krueger on 1/7/26.
//

// Engine/EntryPoint/Win32EntryPoint.cpp (for GUI apps)
#include <VKING/SDK/Prerequisites.hpp>
#include <windows.h>
#include <shellapi.h>   // For CommandLineToArgvW
#include <cstdlib>      // For malloc/free

extern int VKING_Main(int argc, const char** argv);

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
                    PWSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    // Get properly parsed wide-character arguments
    int argc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (wargv == nullptr) {
        return -1;
    }

    // Allocate narrow argv array
    char** argv = static_cast<char**>(malloc(argc * sizeof(char*)));
    if (argv == nullptr) {
        LocalFree(wargv);
        return -1;
    }

    // Zero-initialize in case we need early cleanup
    for (int i = 0; i < argc; ++i) {
        argv[i] = nullptr;
    }

    bool success = true;

    // Convert each wide argument to UTF-8
    for (int i = 0; i < argc; ++i) {
        // Get required buffer size (includes null terminator)
        int size = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1,
                                       nullptr, 0, nullptr, nullptr);
        if (size <= 0) {
            success = false;
            break;
        }

        argv[i] = static_cast<char*>(malloc(static_cast<size_t>(size)));
        if (argv[i] == nullptr) {
            success = false;
            break;
        }

        int written = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1,
                                          argv[i], size, nullptr, nullptr);
        if (written == 0) {
            free(argv[i]);
            argv[i] = nullptr;
            success = false;
            break;
        }
    }

    // Call engine main function only if everything succeeded
    int result = -1;
    if (success) {
        result = VKING_Main(argc, const_cast<const char **>(argv));
    }

    // Cleanup: free all allocated strings
    for (int i = 0; i < argc; ++i) {
        if (argv[i] != nullptr) {
            free(argv[i]);
        }
    }
    free(argv);
    LocalFree(wargv);

    return result;
}