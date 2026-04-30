#pragma once

#include "all_processes.hpp"
#include "processor.hpp"

#include <string>

class System
{
public:
    Processor &Cpu();
    AllProcesses &Processes();
    float MemoryUtilization();
    long UpTime();
    int TotalProcesses();
    int RunningProcesses();
    std::string Kernel();
    std::string OperatingSystem();

private:
    Processor cpu_;
    AllProcesses processes_;
};
