set(CMAKE_AUTOMOC ON)

# Define the library sources
set(PROCESS_STATS_SOURCES
    process_stats.cpp
    process_stats.h
)

# Create the process_stats library as static
# Static linking avoids runtime library path issues in nix builds
add_library(process_stats STATIC ${PROCESS_STATS_SOURCES})

# Link Qt libraries
target_link_libraries(process_stats PUBLIC 
    Qt${QT_VERSION_MAJOR}::Core
)

# Include directories for the library
target_include_directories(process_stats PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/process_stats>
)
