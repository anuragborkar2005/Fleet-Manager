#pragma once

#include "utils/system_parser.hpp"
#include <string>

class System
{
public:
    float MemoryUtilization();
    SystemParser::MemoryInfo DetailedMemory();
    std::vector<double> CpuUtilization();
    std::vector<long> CpuStats();
    long UpTime();
    int TotalCpuCores();

    int TotalProcesses();
    int RunningProcesses();
    std::tuple<double, double, double, int, int, int> LoadAvg();
    std::string Kernel();
    std::string OperatingSystem();
};
