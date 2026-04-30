#include "utils/system_parser.hpp"
#include "monitor/all_processes.hpp"
#include "monitor/system.hpp"

#include <string>

Processor &System::Cpu() { return cpu_; }

AllProcesses &System::Processes() { return processes_; }

std::string System::Kernel() { return std::string(SystemParser::Kernel()); }

float System::MemoryUtilization() { return SystemParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return SystemParser::OperatingSystem(); }

int System::RunningProcesses() { return SystemParser::RunningProcesses(); }

int System::TotalProcesses() { return SystemParser::TotalProcesses(); }
