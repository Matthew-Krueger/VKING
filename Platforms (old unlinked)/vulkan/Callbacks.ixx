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

module;
#include <vulkan/vulkan.h>

export module VKING.Platform.Vulkan:Callbacks;

namespace VKING::Platform::Vulkan {
    // Helper functions – put these in a namespace or as static inline functions

    constexpr size_t HEADER_SIZE = sizeof(size_t);
    constexpr size_t HEADER_ALIGN = alignof(size_t);

    // Returns the pointer to the actual allocated block (header + payload)
    inline void* GetInternalBlock(void* userPtr) {
        return userPtr ? static_cast<char*>(userPtr) - HEADER_SIZE : nullptr;
    }

    // Returns the original requested size that was passed to vkAllocationFunction
    inline size_t GetStoredSize(void* userPtr) {
        void* block = GetInternalBlock(userPtr);
        return block ? *static_cast<size_t*>(block) : 0;
    }

    // Allocates a new block with header, stores the requested size, returns user pointer
    inline void* AllocateWithHeader(const size_t requestedSize, const size_t alignment) {
        if (requestedSize == 0) return nullptr;

        const size_t effectiveAlignment = std::max(alignment, HEADER_ALIGN);
        const size_t totalSize = HEADER_SIZE + requestedSize;
        const size_t roundedTotal = (totalSize + effectiveAlignment - 1) & ~(effectiveAlignment - 1);

        void* block = std::aligned_alloc(effectiveAlignment, roundedTotal);
        if (!block) return nullptr;

        *static_cast<size_t*>(block) = requestedSize;
        return static_cast<char*>(block) + HEADER_SIZE;
    }

    // Frees the block correctly
    inline void FreeWithHeader(void* userPtr) {
        if (userPtr) {
            std::free(GetInternalBlock(userPtr));
        }
    }
}

/// Provide callbacks with C Linkage
extern "C" {
void *VKING_Platform_Vulkan_vkAllocationFunction([[maybe_unused]] void *pUserData,
                                                 const size_t size,
                                                 const size_t alignment,
                                                 [[maybe_unused]] VkSystemAllocationScope allocationScope) {

    return VKING::Platform::Vulkan::AllocateWithHeader(size, alignment);

}

void VKING_Platform_Vulkan_vkFreeFunction([[maybe_unused]] void *pUserData, void *pMemory) {
    VKING::Platform::Vulkan::FreeWithHeader(pMemory);
}


void *VKING_Platform_Vulkan_vkReallocationFunction([[maybe_unused]] void *pUserData,
                                                   void *pOriginal,
                                                   const size_t size,
                                                   const size_t alignment,
                                                   [[maybe_unused]] VkSystemAllocationScope allocationScope) {
    // Case 1: equivalent to allocation
    if (pOriginal == nullptr) {
        return VKING::Platform::Vulkan::AllocateWithHeader(size, alignment);
    }

    // Case 2: free request
    if (size == 0) {
        VKING::Platform::Vulkan::FreeWithHeader(pOriginal);
        return nullptr;
    }

    size_t oldSize = VKING::Platform::Vulkan::GetStoredSize(pOriginal);

    // Allocate new block
    void* newUserPtr = VKING::Platform::Vulkan::AllocateWithHeader(size, alignment);
    if (!newUserPtr) return nullptr;

    // Copy as much as possible (min of old and new size)
    std::memcpy(newUserPtr, pOriginal, std::min(oldSize, size));

    // Free old block
    VKING::Platform::Vulkan::FreeWithHeader(pOriginal);

    return newUserPtr;

}

void VKING_Platform_Vulkan_vkInternalAllocationNotification([[maybe_unused]] void *pUserData,
                                                            [[maybe_unused]] size_t size,
                                                            [[maybe_unused]] VkInternalAllocationType allocationType,
                                                            [[maybe_unused]] VkSystemAllocationScope allocationScope) {
    // do nothing right now
}

void VKING_Platform_Vulkan_vkInternalFreeNotification([[maybe_unused]] void *pUserData, [[maybe_unused]] size_t size,
                                                      [[maybe_unused]] VkInternalAllocationType allocationType,
                                                      [[maybe_unused]] VkSystemAllocationScope allocationScope) {
    // do nothing right now
}

export VkAllocationCallbacks *VKING_Platform_Vulkan_CreateAllocationCallbacks() {
    static VkAllocationCallbacks s_VkAllocationCallbacks = []() {
        VkAllocationCallbacks callbacks{};
        callbacks.pfnAllocation = VKING_Platform_Vulkan_vkAllocationFunction;
        callbacks.pfnReallocation = VKING_Platform_Vulkan_vkReallocationFunction;
        callbacks.pfnFree = VKING_Platform_Vulkan_vkFreeFunction;
        callbacks.pfnInternalAllocation = VKING_Platform_Vulkan_vkInternalAllocationNotification;
        callbacks.pfnInternalFree = VKING_Platform_Vulkan_vkInternalFreeNotification;
        return callbacks;
    }();

    // safe to return the static address as it lives in global memory.
    return &s_VkAllocationCallbacks;
}
}
