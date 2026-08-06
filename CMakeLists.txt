cmake_minimum_required(VERSION 3.14)
project(ProcessStats LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build src first
add_subdirectory(src)

# Tests are opt-out so a cross build can skip them entirely. Without this the
# FetchContent fallback below fires whenever gtest is absent -- which under Nix
# means trying to download googletest inside a sandbox with no network, and the
# build dies on a confusing "downloading ... failed" rather than on the real
# reason (there is no mingw gtest, and a PE test binary could not run here
# anyway).
option(PROCESS_STATS_BUILD_TESTS "Build the process-stats test suite" ON)

if(PROCESS_STATS_BUILD_TESTS)

# GoogleTest setup
# Try to find GoogleTest via find_package first (for Nix and system installations)
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Fall back to FetchContent if GoogleTest is not found
    message(STATUS "GoogleTest not found via find_package, using FetchContent")
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Using system GoogleTest")
endif()

enable_testing()
include(GoogleTest)

# Add tests subdirectory
add_subdirectory(tests)

endif()  # PROCESS_STATS_BUILD_TESTS

# Install rules
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    # iOS: only install the static library
    install(TARGETS process_stats
        ARCHIVE DESTINATION lib
    )
else()
    # Desktop: install library
    install(TARGETS process_stats
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

# Install headers
install(DIRECTORY src/ DESTINATION include/process_stats
    FILES_MATCHING PATTERN "*.h"
)
