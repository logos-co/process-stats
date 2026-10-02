#ifndef PROCESS_STATS_H
#define PROCESS_STATS_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ProcessStats {
    /// All zero when the process cannot be read (exited, or no permission).
    struct ProcessStatsData {
        /// Per core, as top and Activity Monitor report it (one busy core is
        /// 100%), averaged since the previous call for this PID; 0 on the first.
        double cpuPercent = 0.0;
        /// User + system CPU time since the process started.
        double cpuTimeSeconds = 0.0;
        /// Resident set size.
        double memoryMB = 0.0;
    };

    ProcessStatsData getProcessStats(int64_t pid);

    /// @param processes map of module name -> process ID
    /// @return JSON array string; caller must `delete[]` the pointer
    char* getModuleStats(const std::unordered_map<std::string, int64_t>& processes);

    void clearHistory();
}

#endif
