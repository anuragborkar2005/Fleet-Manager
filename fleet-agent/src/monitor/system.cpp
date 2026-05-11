#include "utils/system_parser.hpp"
#include "monitor/system.hpp"

#include <string>

std::string System::Kernel() { return std::string(SystemParser::Kernel()); }

std::vector<double> System::CpuUtilization() { return SystemParser::CpuUtilization(); }

std::vector<long> System::CpuStats() { return SystemParser::CpuStats(); }

int System::TotalCpuCores() { return SystemParser::TotalCpuCores(); }

float System::MemoryUtilization() { return SystemParser::MemoryUtilization(); }

SystemParser::MemoryInfo System::DetailedMemory() { return SystemParser::DetailedMemory(); }

std::string System::OperatingSystem() { return SystemParser::OperatingSystem(); }

int System::RunningProcesses() { return SystemParser::RunningProcesses(); }

int System::TotalProcesses() { return SystemParser::TotalProcesses(); }

long int System::UpTime() { return SystemParser::UpTime(); }

std::tuple<double, double, double, int, int, int> System::LoadAvg() { return SystemParser::LoadAvg(); }
