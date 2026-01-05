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

#include "Signals.hpp"

#include <atomic>

#include "../../../cmake-build-relwithdebinfo/_deps/spdlog-src/include/spdlog/fmt/bundled/chrono.h"


#ifdef _WIN32
#include <Windows.h>
#else

extern "C"{
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
}
#endif

namespace VKING::Shutdown {
    static constexpr size_t MAX_MESSAGE_LENGTH = 100;
    static char s_MessageBuf[MAX_MESSAGE_LENGTH]{};
    static Info s_Info{Reason::REASON_NONE, s_MessageBuf};
    static std::atomic_bool s_ShutdownRequested{false};
}

/**
* Interrupt Handler, C linkage so they won't get mangled
*/
extern "C" {

    void interruptHandler(int signal) {
        switch (signal) {
            case SIGINT:
                // "[SIGINT] Interrupt subscribed and received."
                request(VKING::Shutdown::Reason::REASON_SIGINT, nullptr);
                break;
            case SIGTERM:
                // "[SIGTERM] Interrupt was subscribed and received."
                request(VKING::Shutdown::Reason::REASON_SIGTERM, nullptr);
                break;
            default:
                // "[SIG Unknown] Interrupt was subscribed and received, but no handler."
                request(VKING::Shutdown::Reason::REASON_UNKNOWN, nullptr);
                break;
        }
    }
}

namespace VKING::Shutdown {
    void request(Reason reason, const char* message) {
        // safely memcpy the message into our buffer, checking strlen first
        if (message) {
            size_t messageLength = strnlen(message, MAX_MESSAGE_LENGTH - 1);
            memcpy(s_MessageBuf, message, messageLength);
            s_MessageBuf[messageLength] = '\0';
        } else {
            s_MessageBuf[0] = '\0';
        }

        // copy the reason
        s_Info.reason = reason;

        // and raise the flag
        // if multiple requests are raised, the last one in is fine
        // we care about *read* inconsistencies in the atomic
        s_ShutdownRequested.store(true, std::memory_order_release);
    }

    bool isRequested() {
        return s_ShutdownRequested.load(std::memory_order_acquire);
    }

    bool restartRequested() {
        if (!isRequested()) return false;
        if (s_Info.reason == Reason::REASON_USER_RESTART) return true;
        if (s_Info.reason == Reason::REASON_INVOLUNTARY_RESTART) return true;
        return false;
    }

    const Info &getReason() {
        return s_Info;
    }

    void clearRequest() {
        s_ShutdownRequested.store(false, std::memory_order_release);
        s_Info.reason = Reason::REASON_NONE;
        s_MessageBuf[0] = '\0';
    }


#ifdef _WIN32
    void registerInterruptHandler() {
#pragma message("VKING::Shutdown: Windows console control handlers are not fully implemented. " \
"Application might not quit gracefully via all console commands (e.g., closing console window).")
        signal(SIGINT, interruptHandler);
        signal(SIGTERM, interruptHandler);
    }
#else// we are assuming a posix system

    void registerInterruptHandler() {
        struct sigaction sa{};
        sa.sa_handler = interruptHandler; // Your existing C-linkage handler
        sigemptyset(&sa.sa_mask);         // Clear the mask of signals to block
        sigaddset(&sa.sa_mask, SIGINT);
        sigaddset(&sa.sa_mask, SIGTERM);
        sa.sa_flags = SA_RESTART;         // Restart interrupted system calls (e.g., read, write)

        // Register handler for SIGINT (Ctrl+C)
        if (sigaction(SIGINT, &sa, nullptr) == -1) {
            // Log error in an async-signal-safe way if this function were called from a signal handler
            // For startup, std::cerr is generally acceptable, but write() is safer.
            // Using a simple write here for consistency in messaging philosophy.
            const char* msg = "VKING::Shutdown ERROR: Could not register SIGINT handler.\n";
            write(STDERR_FILENO, msg, strlen(msg));
        }

        // Register handler for SIGTERM
        if (sigaction(SIGTERM, &sa, nullptr) == -1) {
            const char* msg = "VKING::Shutdown ERROR: Could not register SIGTERM handler.\n";
            write(STDERR_FILENO, msg, strlen(msg));
        }

    }
#endif


}
