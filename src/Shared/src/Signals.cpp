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

    void registerInterruptHandler() {
        signal(SIGINT, interruptHandler);
        signal(SIGTERM, interruptHandler);
    }

}
