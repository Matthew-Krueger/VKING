# cmake/Finddynlib.cmake  (or name it dynlib-fetch.cmake, whatever you prefer)

include(FetchContent)

# Allow the user to specify the tag/version, default to the latest known release
set(DYNLIB_GIT_TAB v3.0.1 CACHE STRING "dynlib git tag/branch/commit to fetch")

message(STATUS "Fetching dynlib ${dynlib_GIT_TAG} from GitHub via FetchContent")

FetchContent_Declare(
        dynlib
        GIT_REPOSITORY https://github.com/martin-olivier/dylib
        GIT_TAG        ${DYNLIB_GIT_TAB}
        # Optional: use OVERRIDE_FIND_PACKAGE if CMake >= 3.24 for cleaner find_package integration
        # OVERRIDE_FIND_PACKAGE
)

# This will download, configure, and make the dynlib::dynlib target available
FetchContent_MakeAvailable(dynlib)