#include <gtest/gtest.h>
#include "process_stats.h"
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

class ProcessStatsTest : public ::testing::Test {
protected:
    std::vector<pid_t> m_childPids;

    void SetUp() override
    {
        ProcessStats::clearHistory();
    }

    void TearDown() override
    {
        ProcessStats::clearHistory();
        for (pid_t pid : m_childPids) {
            if (pid > 0) {
                kill(pid, SIGTERM);
                int st = 0;
                waitpid(pid, &st, 0);
            }
        }
        m_childPids.clear();
    }

    /// Spawn `sleep 10` child; returns -1 if fork fails. Caller should ASSERT_GT(pid, 0).
    pid_t spawnSleepChild()
    {
        pid_t pid = fork();
        if (pid < 0)
            return -1;
        if (pid == 0) {
            execlp("sleep", "sleep", "10", nullptr);
            std::perror("execlp sleep");
            _exit(127);
        }
        m_childPids.push_back(pid);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return pid;
    }
};

TEST_F(ProcessStatsTest, GetProcessStats_ReturnsZeroedStatsForNegativePid)
{
    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(-1);

    EXPECT_EQ(stats.cpuPercent, 0.0);
    EXPECT_EQ(stats.cpuTimeSeconds, 0.0);
    EXPECT_EQ(stats.memoryMB, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_ReturnsZeroedStatsForZeroPid)
{
    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(0);

    EXPECT_EQ(stats.cpuPercent, 0.0);
    EXPECT_EQ(stats.cpuTimeSeconds, 0.0);
    EXPECT_EQ(stats.memoryMB, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_ReturnsValidStatsForCurrentProcess)
{
    int64_t currentPid = static_cast<int64_t>(getpid());

    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(currentPid);

    EXPECT_GT(stats.memoryMB, 0.0);
    EXPECT_GE(stats.cpuTimeSeconds, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_MemoryIsNonNegative)
{
    int64_t currentPid = static_cast<int64_t>(getpid());

    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(currentPid);

    EXPECT_GE(stats.memoryMB, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_CpuTimeIsNonNegative)
{
    int64_t currentPid = static_cast<int64_t>(getpid());

    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(currentPid);

    EXPECT_GE(stats.cpuTimeSeconds, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_CpuPercentIsZeroOnFirstCall)
{
    int64_t currentPid = static_cast<int64_t>(getpid());

    ProcessStats::clearHistory();

    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(currentPid);

    EXPECT_EQ(stats.cpuPercent, 0.0);
}

TEST_F(ProcessStatsTest, GetProcessStats_CpuPercentUpdatesOnSecondCall)
{
    int64_t currentPid = static_cast<int64_t>(getpid());

    ProcessStats::getProcessStats(currentPid);

    volatile double sum = 0.0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i * 0.1;
    }

    usleep(10000);

    ProcessStats::ProcessStatsData stats = ProcessStats::getProcessStats(currentPid);

    EXPECT_GE(stats.cpuPercent, 0.0);
}

TEST_F(ProcessStatsTest, GetModuleStats_ReturnsEmptyArrayWhenNoPlugins)
{
    std::unordered_map<std::string, int64_t> emptyProcesses;
    char* result = ProcessStats::getModuleStats(emptyProcesses);

    ASSERT_NE(result, nullptr);

    nlohmann::json doc = nlohmann::json::parse(result);

    EXPECT_TRUE(doc.is_array());
    EXPECT_EQ(doc.size(), 0u);

    delete[] result;
}

TEST_F(ProcessStatsTest, GetModuleStats_ReturnsNonNullPointer)
{
    std::unordered_map<std::string, int64_t> emptyProcesses;
    char* result = ProcessStats::getModuleStats(emptyProcesses);

    ASSERT_NE(result, nullptr);

    delete[] result;
}

TEST_F(ProcessStatsTest, GetModuleStats_ReturnsValidJsonStructure)
{
    pid_t pid = spawnSleepChild();
    ASSERT_GT(pid, 0);

    std::unordered_map<std::string, int64_t> processes;
    processes["test_plugin"] = static_cast<int64_t>(pid);

    char* result = ProcessStats::getModuleStats(processes);

    ASSERT_NE(result, nullptr);

    nlohmann::json doc = nlohmann::json::parse(result);

    EXPECT_TRUE(doc.is_array());
    ASSERT_EQ(doc.size(), 1u);

    auto moduleObj = doc[0];
    EXPECT_TRUE(moduleObj.contains("name"));
    EXPECT_TRUE(moduleObj.contains("pid"));
    EXPECT_TRUE(moduleObj.contains("cpu_percent"));
    EXPECT_TRUE(moduleObj.contains("cpu_time_seconds"));
    EXPECT_TRUE(moduleObj.contains("memory_mb"));

    EXPECT_EQ(moduleObj["name"].get<std::string>(), "test_plugin");
    EXPECT_EQ(moduleObj["pid"].get<int64_t>(), static_cast<int64_t>(pid));
    EXPECT_GE(moduleObj["cpu_percent"].get<double>(), 0.0);
    EXPECT_GE(moduleObj["cpu_time_seconds"].get<double>(), 0.0);
    EXPECT_GE(moduleObj["memory_mb"].get<double>(), 0.0);

    delete[] result;
}

TEST_F(ProcessStatsTest, GetModuleStats_IncludesAllPassedProcesses)
{
    pid_t pid1 = spawnSleepChild();
    pid_t pid2 = spawnSleepChild();
    ASSERT_GT(pid1, 0);
    ASSERT_GT(pid2, 0);

    std::unordered_map<std::string, int64_t> processes;
    processes["plugin_one"] = static_cast<int64_t>(pid1);
    processes["plugin_two"] = static_cast<int64_t>(pid2);

    char* result = ProcessStats::getModuleStats(processes);

    ASSERT_NE(result, nullptr);

    nlohmann::json doc = nlohmann::json::parse(result);

    EXPECT_TRUE(doc.is_array());

    ASSERT_EQ(doc.size(), 2u);

    std::set<std::string> names;
    for (const auto& val : doc) {
        names.insert(val["name"].get<std::string>());
    }

    EXPECT_TRUE(names.count("plugin_one"));
    EXPECT_TRUE(names.count("plugin_two"));

    delete[] result;
}

TEST_F(ProcessStatsTest, GetModuleStats_SkipsInvalidPids)
{
    pid_t validPid = spawnSleepChild();
    ASSERT_GT(validPid, 0);

    std::unordered_map<std::string, int64_t> processes;
    processes["valid_plugin"] = static_cast<int64_t>(validPid);
    processes["invalid_plugin"] = -1;
    processes["zero_plugin"] = 0;

    char* result = ProcessStats::getModuleStats(processes);

    ASSERT_NE(result, nullptr);

    nlohmann::json doc = nlohmann::json::parse(result);

    EXPECT_TRUE(doc.is_array());

    ASSERT_EQ(doc.size(), 1u);

    EXPECT_EQ(doc[0]["name"].get<std::string>(), "valid_plugin");

    delete[] result;
}
