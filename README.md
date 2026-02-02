# Process Stats Library

A cross-platform C++ library for monitoring process CPU and memory statistics.

## Building

### With Nix

```bash
nix build
```

### With CMake

```bash
mkdir build && cd build
cmake .. -GNinja
ninja
```

## Running Tests

```bash
# With Nix
nix build .#process-stats-tests
./result/bin/process_stats_tests

# With CMake
cd build
ninja process_stats_tests
./bin/process_stats_tests
```

## API

```cpp
#include <process_stats/process_stats.h>

// Get stats for a single process
ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(pid);
// stats.cpuPercent - CPU usage percentage
// stats.cpuTimeSeconds - Total CPU time in seconds
// stats.memoryMB - Memory usage in megabytes

// Get stats for multiple processes as JSON
QHash<QString, qint64> processes;
processes["my_process"] = pid;
char* json = ProcessStats::getModuleStats(processes);
// Returns: [{"name":"my_process","cpu_percent":1.5,"cpu_time_seconds":10.2,"memory_mb":45.3}]
delete[] json;

// Clear internal CPU time history (useful for tests)
ProcessStats::clearHistory();
```
