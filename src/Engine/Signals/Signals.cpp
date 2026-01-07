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
#include <unistd.h>
}
#endif

namespace VKING::Shutdown {
    static constexpr size_t MAX_MESSAGE_LENGTH = 100;
    static char s_MessageBuf[MAX_MESSAGE_LENGTH]{};
    static std::atomic<Reason> s_RequestReason = Reason::REASON_NONE;
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
                request(VKING::Shutdown::Reason::REASON_SIGINT, "[SIGINT] Signal Interrupt received and interrupt handled.");
                break;
            case SIGTERM:
                // "[SIGTERM] Interrupt was subscribed and received."
                request(VKING::Shutdown::Reason::REASON_SIGTERM, "[SIGTERM] Signal Interrupt received and interrupt handled.");
                break;
            default:
                // "[SIG Unknown] Interrupt was subscribed and received, but no handler."
                request(VKING::Shutdown::Reason::REASON_UNKNOWN, "[UNKNOWN] Signal Interrupt received but no handler was known.");
                break;
        }
    }
}

namespace VKING::Shutdown {
    void request(Reason reason, const char* message) {

        if (message) {
            // safely copy byte for byte our message into the buffer
            // memcpy must NOT be used as it is not strictly async safe
            // the message may still be corrupted if multiple interrupts happen, but at that point, meh
            for (uint32_t i = 0; i < MAX_MESSAGE_LENGTH - 1; i++) {
                const char c = message[i];
                s_MessageBuf[i] = c;
                if (c == '\0') break;
            }
            s_MessageBuf[MAX_MESSAGE_LENGTH - 1] = '\0';  // ensure null-termination
        } else {
            s_MessageBuf[0] = '\0';
        }

        // copy the reason
        s_RequestReason.store(reason, std::memory_order_release);

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
        const Reason reason = s_RequestReason.load(std::memory_order_acquire);
        if ( reason == Reason::REASON_USER_RESTART) return true;
        if ( reason == Reason::REASON_INVOLUNTARY_RESTART) return true;
        return false;
    }

    Info getReason() {

        // construct a string so the lifetime is separate and remains separable in the thread
        // and cap the length to MAX_MESSAGE_LENGTH, just in case the message was not properly terminated
        auto msg = std::string(s_MessageBuf, std::min(strlen(s_MessageBuf), MAX_MESSAGE_LENGTH));
        const Reason reason = s_RequestReason.load(std::memory_order_acquire);

        return {
            reason, std::move(msg)
        };

    }

    void clearRequest() {
        s_ShutdownRequested.store(false, std::memory_order_release);
        s_RequestReason.store(Reason::REASON_NONE, std::memory_order_release);
        memset(s_MessageBuf, '\0', MAX_MESSAGE_LENGTH);
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
            // Using spmple writing here for consistency in messaging philosophy.
            const auto msg = "VKING::Shutdown ERROR: Could not register SIGINT handler.\n";
            write(STDERR_FILENO, msg, strlen(msg));
        }

        // Register handler for SIGTERM
        if (sigaction(SIGTERM, &sa, nullptr) == -1) {
            const auto msg = "VKING::Shutdown ERROR: Could not register SIGTERM handler.\n";
            write(STDERR_FILENO, msg, strlen(msg));
        }

    }
#endif


}
