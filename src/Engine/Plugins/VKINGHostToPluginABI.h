/**
 * @file PluginABIBaseSpec.h
 * @brief Top-level host ABI specification passed to VKING plugins.
 *
 * This header defines the root ABI structure that the VKING host (engine core /
 * executable) passes to dynamically loaded plugins during plugin initialization.
 *
 * The goal of this structure is to provide plugins access to host services
 * (e.g., logging) via stable C function tables without linking against host C++
 * symbols or the C++ standard library.
 *
 * ABI Stability Rules:
 * - This header is intended to be usable from C99 and C++.
 * - Do not add C++ types (std::string, std::vector, references, exceptions).
 * - Do not reorder fields. Extend by appending new fields at the end.
 * - Use `abiVersion` and `structSize` to allow forward/backward compatibility checks.
 *
 * Lifetime:
 * - The host owns the `VKING_ABI_SPEC` instance and all nested API tables it
 *   points to.
 * - Plugins may store pointers received via this struct for as long as the host
 *   guarantees they remain valid (typically the lifetime of the process or until
 *   plugin shutdown).
 */

#ifndef VKING_PLUGIN_ABI_BASE_SPEC_H
#define VKING_PLUGIN_ABI_BASE_SPEC_H

#include <stdint.h>

#include "Logging/LoggerStableCABI.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct VKING_Hostside_API
 * @brief Root ABI table provided by the host to plugins.
 *
 * This struct is the primary entry point for host-provided services. It is
 * commonly passed to a plugin initialization function such as:
 *
 * @code
 * VKING_CALL bool VKING_PluginInit(const VKING_ABI_SPEC* host);
 * @endcode
 *
 * Versioning:
 * - `abiVersion` describes the major version of the host ABI.
 * - `structSize` is the size (in bytes) of the host-provided `VKING_ABI_SPEC`
 *   instance. This supports ABI extension by appending new fields.
 *
 * Forward compatibility guideline for plugins:
 * - Verify `host->abiVersion` is a version you support.
 * - Verify `host->structSize` is at least large enough to contain the fields
 *   you intend to use.
 *
 * @note Plugins should treat this structure as read-only.
 */
typedef struct VKING_Hostside_API {
    /**
     * @brief Host ABI major version.
     *
     * Plugins should reject hosts with an ABI version they do not support.
     * Increment this value only for breaking ABI changes.
     */
    uint32_t abiVersion;

    /**
     * @brief Size in bytes of the `VKING_ABI_SPEC` struct instance provided by the host.
     *
     * This enables ABI extension. A newer host may provide a larger `structSize`
     * with additional fields appended to the end of this struct.
     */
    uint32_t structSize;

    /**
     * @brief Pointer to the host-provided logging API table.
     *
     * This table allows plugins to emit log messages to the host's logging system
     * without depending on the host's internal logging library (e.g., spdlog).
     *
     * Ownership and lifetime:
     * - Owned by the host.
     * - Valid for at least the lifetime of the plugin after successful initialization,
     *   unless the host explicitly documents otherwise.
     *
     * @note May be NULL if the host does not provide logging (plugins should handle this).
     */
    const VKING_Hostside_Logging_API *loggingAPISpec;

    /* Future host services should be appended here, e.g.:
     * - const VKING_Filesystem_API* filesystem;
     * - const VKING_Allocator_API* allocator;
     * - const VKING_Threading_API* threading;
     */
} VKING_Hostside_API;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VKING_PLUGIN_ABI_BASE_SPEC_H */
