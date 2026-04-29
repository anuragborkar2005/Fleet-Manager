#pragma once

#include <string>
#include <vector>

namespace SystemParser
{
    float MemoryUtilization();
    long UpTime();
    std::vector<int> Pids;
    int TotalProcesses();
    int RunningProcesses();
    std::string OperatingSystem();
    std::string Kernel();
    std::string UserByID(int);
    std::vector<std::string> CpuUtilization();

}