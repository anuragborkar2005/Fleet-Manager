#pragma once

#include <string>
#include <vector>

namespace SystemParser
{
    float MemoryUtilization();
    long UpTime();
    std::vector<int> Pids();
    int TotalProcesses();
    int RunningProcesses();
    std::string OperatingSystem();
    std::string Kernel();
    std::string UserByID(int);
    struct MemoryInfo
    {
        long total;
        long free;
        long available;
        long buffers;
        long cached;
        long swap_total;
        long swap_free;
    };
    MemoryInfo DetailedMemory();
    std::vector<double> CpuUtilization();
    std::tuple<double, double, double, int, int, int> LoadAvg();
    std::vector<long> CpuStats();
    int TotalCpuCores();
}
