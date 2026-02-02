set(CMAKE_AUTOMOC ON)

# Define the library sources
set(PROCESS_STATS_SOURCES
    process_stats.cpp
    process_stats.h
)

# Create the process_stats library
# Build as static for iOS (dynamic libraries not supported in iOS apps), shared otherwise
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    add_library(process_stats STATIC ${PROCESS_STATS_SOURCES})
else()
    add_library(process_stats SHARED ${PROCESS_STATS_SOURCES})
endif()

# Link Qt libraries
target_link_libraries(process_stats PUBLIC 
    Qt${QT_VERSION_MAJOR}::Core
)

# Include directories for the library
target_include_directories(process_stats PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/process_stats>
)

# Skip RPATH settings for iOS (not applicable)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    # Set RPATH settings for the library
    if(APPLE)
        set_target_properties(process_stats PROPERTIES
            INSTALL_RPATH "@loader_path"
            INSTALL_NAME_DIR "@rpath"
            BUILD_WITH_INSTALL_NAME_DIR TRUE
            BUILD_WITH_INSTALL_RPATH TRUE)
    else()
        set_target_properties(process_stats PROPERTIES
            INSTALL_RPATH "$ORIGIN"
            BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
endif()
