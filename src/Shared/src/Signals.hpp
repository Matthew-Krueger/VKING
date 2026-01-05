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

    struct Info {
        Reason reason;
        const char* message;
    };

    void request(Reason reason, const char* message = nullptr);

    bool isRequested();
    bool restartRequested();
    const Info &getReason();
    void clearRequest();

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

    void registerInterruptHandler();
}