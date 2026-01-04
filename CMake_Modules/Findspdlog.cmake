# cmake/Findspdlog.cmake  (or name it spdlog-fetch.cmake, whatever you prefer)

include(FetchContent)

# Allow the user to specify the tag/version, default to the latest known release
set(SPDLOG_GIT_TAG v1.17.0 CACHE STRING "spdlog git tag/branch/commit to fetch")

message(STATUS "Fetching spdlog ${SPDLOG_GIT_TAG} from GitHub via FetchContent")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        ${SPDLOG_GIT_TAG}
        # Optional: use OVERRIDE_FIND_PACKAGE if CMake >= 3.24 for cleaner find_package integration
        # OVERRIDE_FIND_PACKAGE
)

# This will download, configure, and make the spdlog::spdlog target available
FetchContent_MakeAvailable(spdlog)