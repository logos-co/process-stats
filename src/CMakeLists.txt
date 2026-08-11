# Define the library sources
set(PROCESS_STATS_SOURCES
    process_stats.cpp
    process_stats.h
)

add_library(process_stats STATIC ${PROCESS_STATS_SOURCES})

target_link_libraries(process_stats PUBLIC
    nlohmann_json::nlohmann_json
)

# GetProcessMemoryInfo lives in psapi. GetProcessTimes/OpenProcess come from
# kernel32, which mingw links by default.
if(WIN32)
    target_link_libraries(process_stats PUBLIC psapi)
endif()

target_include_directories(process_stats PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/process_stats>
)
