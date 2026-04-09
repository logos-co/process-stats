#ifndef PROCESS_STATS_H
#define PROCESS_STATS_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ProcessStats {
    struct ProcessStatsData {
        double cpuPercent = 0.0;
        double cpuTimeSeconds = 0.0;
        double memoryMB = 0.0;
    };

    ProcessStatsData getProcessStats(int64_t pid);

    /// @param processes map of module name -> process ID
    /// @return JSON array string; caller must `delete[]` the pointer
    char* getModuleStats(const std::unordered_map<std::string, int64_t>& processes);

    void clearHistory();
}

#endif
