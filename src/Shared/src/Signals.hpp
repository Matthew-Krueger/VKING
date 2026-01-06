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
// Created by Matthew Krueger on 1/5/26.
//

#pragma once

namespace VKING::Shutdown {

    /**
     * @brief Enum class representing various reasons for a shutdown or restart event.
     *
     * This enum class is used to categorize and specify the cause of a shutdown
     * or restart operation. It covers both user-initiated and system-triggered events.
     *
     * @note Only REASON_USER_RESTART and REASON_INVOLUNTARY_RESTART are restart reasons.
     *
     */
    enum class Reason {
        REASON_USER_REQUEST,
        REASON_USER_RESTART,
        REASON_INVOLUNTARY_RESTART,
        REASON_SIGTERM,
        REASON_SIGINT,
        REASON_SIGBREAK,
        REASON_FATAL_ERROR,
        REASON_UNKNOWN,
        REASON_SIG_UNHANDLED,
        REASON_NONE
    };

    /**
     * @brief Class representing info on why a shutdown was requested
     *
     * @note The string is returned as a copy, so the caller is responsible for managing its lifetime. Calling reset will not invalidate the string.
     */
    struct Info {
        Reason reason;
        std::string message;
    };

    /**
     * @brief Requests a shutdown at the next safe shutdown time.
     *
     * @note This function is async-singal-safe.
     *
     * @param reason The reason the caller is requesting the shutdown
     * @param message The message the caller would like to go along with the shutdown, if any. You may pass nullptr
     */
    void request(Reason reason, const char* message = nullptr);

    /**
     * @brief Tests if a shutdown is requested
     * @return Whether or not a shutdown is requested
     */
    bool isRequested();

    /**
     * @brief Tests if a restart is requested
     * @return Whether or not a restart is requested
     */
    bool restartRequested();

    /**
     * @brief Gets the reason for the shutdown
     *
     * @note Lifetime of all objects are transferred. Please remember to call clearRequest(); it is not cleared automatically
     *
     * @return The reason for the shutdown
     */
    Info getReason();

    /**
     * @brief Clears the stored request
     */
    void clearRequest();

    /**
     * @brief Converts a Reason enum value to its corresponding string representation.
     *
     * This function maps each Reason enum value to a human-readable string describing
     * the reason for a shutdown or restart event.
     *
     * @param reason The Reason enum value to be converted to a string.
     * @return A constant character pointer to the string representation of the given reason.
     *         If the reason is not recognized, "Unknown" will be returned.
     */
    inline const char * reasonToString(Reason reason) {
        switch (reason) {
            case Reason::REASON_USER_REQUEST: return "User Requested";
            case Reason::REASON_USER_RESTART: return "User Restarted";
            case Reason::REASON_INVOLUNTARY_RESTART: return "Involuntary Restart";
            case Reason::REASON_SIGTERM: return "SIGTERM";
            case Reason::REASON_SIGINT: return "SIGINT";
            case Reason::REASON_SIGBREAK: return "SIGBREAK";
            case Reason::REASON_FATAL_ERROR: return "Fatal Error";
            case Reason::REASON_NONE: return "No reason for shutdown. If you see this error, something is wrong.";
            case Reason::REASON_SIG_UNHANDLED: return "Signal was received, but no handler was registered.";
            case Reason::REASON_UNKNOWN:
            default: return "Unknown";
        }
    }

    /**
     * @brief Registers a signal handler to handle certain signals and other events
     *
     * This does not override the native window's handling of events, and is only intended to capture terminal events.
     */
    void registerInterruptHandler();
}