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

#ifndef VKING_CALLDEF_H
#define VKING_CALLDEF_H


// VKING_CALL: calling convention for function pointers / exported functions
#if defined(_WIN32) || defined(__CYGWIN__)
  #define VKING_CALL __cdecl
#else
  #define VKING_CALL
#endif

// VKING_EXPORT: symbol visibility for the one exported plugin entrypoint symbol
// Define VKING_BUILDING_PLUGIN when compiling the plugin shared library itself.
// If the host loads plugins via dlopen/GetProcAddress, it does not need dllimport;
// leaving VKING_EXPORT empty for the host side is fine.
//
// If you *do* want dllimport for host builds on Windows, define VKING_USING_PLUGIN
// when compiling the host and use the VKING_IMPORT macro below.
#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(VKING_BUILDING_PLUGIN)
    #define VKING_EXPORT __declspec(dllexport)
  #else
    #define VKING_EXPORT /* host usually uses GetProcAddress; no import needed */
  #endif
#else
  // GCC/Clang visibility attribute (also works on macOS with clang)
  #if defined(__GNUC__) || defined(__clang__)
    #define VKING_EXPORT __attribute__((visibility("default")))
  #else
    #define VKING_EXPORT
  #endif
#endif

// Optional: explicit import macro if you ever link against an import library on Windows.
#if defined(_WIN32) || defined(__CYGWIN__)
  #define VKING_IMPORT __declspec(dllimport)
#else
  #define VKING_IMPORT
#endif

#endif
