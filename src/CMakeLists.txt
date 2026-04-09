# Define the library sources
set(PROCESS_STATS_SOURCES
    process_stats.cpp
    process_stats.h
)

add_library(process_stats STATIC ${PROCESS_STATS_SOURCES})

target_link_libraries(process_stats PUBLIC
    nlohmann_json::nlohmann_json
)

target_include_directories(process_stats PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/process_stats>
)
