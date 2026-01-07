
include(FetchContent)

# Allow the user to specify the tag/version, default to the latest known release
set(FMT_GIT_TAG 12.1.0 CACHE STRING "fmt git tag/branch/commit to fetch")

message(STATUS "Fetching fmt ${FMT_GIT_TAG} from GitHub via FetchContent")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        ${FMT_GIT_TAG}
        # Optional: use OVERRIDE_FIND_PACKAGE if CMake >= 3.24 for cleaner find_package integration
        # OVERRIDE_FIND_PACKAGE
)

# This will download, configure, and make the fmt targets available
FetchContent_MakeAvailable(fmt)