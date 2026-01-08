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
#include <csignal>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#else

extern "C" {
#include <signal.h>
#include <unistd.h>
}
#endif

namespace VKING::Shutdown {
    namespace detail {
        // ---- Signal-safe state ----
        // Per the posix spec you can *only* raise a sig_atomic_t, and no other types are async-signal-safe,
        // HOWEVER, sig_atomic_t is NOT atomic, but there is nothing we can really do about that
        // on any platform we'd ever want to run at least 32 bit integers are already memory order relaxed at a minimum
        // which strictly speaking is good enough.
        static volatile sig_atomic_t s_ShutdownRequestedSignalSafe{false};
        static volatile sig_atomic_t s_RequestReasonSignalSafe = static_cast<sig_atomic_t>(Reason::REASON_NONE);

        // ---- Thread-safe state ----
        static std::atomic_bool s_ShutdownRequestedThreadSafe{false};
        static std::atomic s_RequestReasonThreadSafe{Reason::REASON_NONE};

        // ---- Message (normal code only) ----
        std::mutex s_RequestReasonMutex;
        static std::string s_Message{};
    }


    // INTERNAL — signal handler only
    // instead of copying into the message buff we will defer the copy
    // until we are returning the reason
    static void requestFromSignal(Reason reason) {
        if (detail::s_ShutdownRequestedSignalSafe) return; // first signal wins

        detail::s_RequestReasonSignalSafe = static_cast<sig_atomic_t>(reason);
        detail::s_ShutdownRequestedSignalSafe = 1;
    }
}

/**
* Interrupt Handler, C linkage so they won't get mangled
*/
extern "C" {
void interruptHandler(const int signal) {
    switch (signal) {
        case SIGINT:
            // "[SIGINT] Interrupt subscribed and received."
            VKING::Shutdown::requestFromSignal(VKING::Shutdown::Reason::REASON_SIGINT);
            break;
        case SIGTERM:
            // "[SIGTERM] Interrupt was subscribed and received."
            VKING::Shutdown::requestFromSignal(VKING::Shutdown::Reason::REASON_SIGTERM);
            break;
        default:
            // "[SIG Unknown] Interrupt was subscribed and received, but no handler."
            VKING::Shutdown::requestFromSignal(VKING::Shutdown::Reason::REASON_UNKNOWN);
            break;
    }
}
}

namespace VKING::Shutdown {
    void request(Reason reason, const char *message) {
        // ReSharper disable once CppTooWideScopeInitStatement
        bool expected = false;
        if (!detail::s_ShutdownRequestedThreadSafe.compare_exchange_strong(expected, true, std::memory_order_release)) {
            return; // first request wins
        }

        detail::s_RequestReasonThreadSafe.store(reason, std::memory_order_release);

        {
            std::lock_guard guard(detail::s_RequestReasonMutex);
            detail::s_Message = message ? message : "";
        }
    }

    bool isRequested() {
        // check both the async handler and the thread handler
        return detail::s_ShutdownRequestedThreadSafe.load(std::memory_order_acquire) ||
               detail::s_ShutdownRequestedSignalSafe;
    }

    bool restartRequested() {
        // first check if we even have a shutdown request in
        if (!isRequested()) return false;

        // I know its three atomic reads but I don't care this is not a hot path
        if (detail::s_ShutdownRequestedSignalSafe) {
            // we know it was a signal safe, and *AS OF RIGHT NOW* no signals restart, so we'll return false
            return false;
        }

        if (detail::s_ShutdownRequestedThreadSafe) {
            // the thread safe handler requested the shutdown

            auto reason = detail::s_RequestReasonThreadSafe.load(std::memory_order_acquire);
            if (reason == Reason::REASON_USER_RESTART || reason == Reason::REASON_INVOLUNTARY_RESTART) return true;
            // if restart was user request or involuntary
            return false; // otherwise it was not requested
        }

        // should be unreachable but we know how code is
        // if it gets here we have no clue what the heck is going on
        return false;
    }

    Info getReason() {
        const bool signalSafeRequested = static_cast<bool>(detail::s_ShutdownRequestedSignalSafe);
        const bool threadSafeRequested = detail::s_ShutdownRequestedThreadSafe.load(std::memory_order_acquire);

        // first check the interrupts, since that requires construction of the reason string
        if (signalSafeRequested) {
            // shutdown was requested. Check for the reason and construct the message.
            // Will be sort of inefficient, but we need the state both in here and returned
            // doesn't matter much since getting interrupts (or shutdown requests for that matter)
            // is not a hot code path

            // since signal always wins, we can just straight up return
            // we do not have to rebuild the static
            switch (const auto reason = static_cast<Reason>(detail::s_RequestReasonSignalSafe)) {
                case Reason::REASON_SIGINT:
                    return { .reason = reason, .message = "[SIGINT] Interrupt Request Received. Reason: SIGINT"};
                case Reason::REASON_SIGTERM:
                    return {.reason = reason, .message = "[SIGTERM] Interrupted Request. Reason: SIGTERM"};
                case Reason::REASON_UNKNOWN:
                default:
                    return {.reason = reason, .message = "[UNKNOWN] Unknown Reason. Reason: UNKNOWN"};
            }

            // we will leave the flags set as they are and continue
            // since this behavior is deterministic, we can just simply return Info

        }

        if (threadSafeRequested) {
            // thread safe shutdown called. just construct the result in place

            // all we need to do is acquire the mutex to copy it, then return.
            std::lock_guard guard(detail::s_RequestReasonMutex);
            return {
                .reason = detail::s_RequestReasonThreadSafe.load(std::memory_order_acquire),
                .message = detail::s_Message,
            };
        };

        // There *was* no shutdown request that happened
        // so either the caller did not think to check whether the shutdown was requested, or cleared it,
        // or cosmic rays corrupted your ram. COOL!
        return {
            .reason = Reason::REASON_NONE,
            .message =
            "VKING has no clue what happened! VKING::Shutdown::getReason() was called, but there is no reason for a shutdown. Either the caller did not think to check whether they should call this function, or they prematurely cleared it, or a cosmic ray hit your ram. COOL!"
        };
    }

    void clearRequest() {

        // some debate over blocking delivery of signals while we reset them
        // I don't think that's required here, since the worst that will happen
        // is a torn write between s_ShutdownRequestedSignalSafe and s_RequestReasonSignalSafe.
        // so.... meh

        // we are now safe to work on the async system
        detail::s_RequestReasonSignalSafe = static_cast<volatile sig_atomic_t>(Reason::REASON_NONE);
        detail::s_ShutdownRequestedSignalSafe = false; // we no longer want it
        // we have no reason to shutdown

        // next, clear the thread safe
        detail::s_RequestReasonThreadSafe.store(Reason::REASON_NONE, std::memory_order_release);
        detail::s_ShutdownRequestedThreadSafe.store(false, std::memory_order_release);
        // and clear the string
        {
            std::lock_guard guard(detail::s_RequestReasonMutex);
            detail::s_Message.clear();
        }

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
        sigemptyset(&sa.sa_mask); // Clear the mask of signals to block
        sigaddset(&sa.sa_mask, SIGINT);
        sigaddset(&sa.sa_mask, SIGTERM);
        sa.sa_flags = SA_RESTART; // Restart interrupted system calls (e.g., read, write)

        // Register handler for SIGINT (Ctrl+C)

        if (sigaction(SIGINT, &sa, nullptr) == -1) {
            // Log error in an async-signal-safe way if this function were called from a signal handler
            // For startup, std::cerr is generally acceptable, but write() is safer.
            // Using simple writing here for consistency in messaging philosophy.
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
