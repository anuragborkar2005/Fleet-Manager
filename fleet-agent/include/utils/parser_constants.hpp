#pragma once

#include <string>

namespace ParserConstants
{
    constexpr std::string ProcDirectory{"/proc"};
    constexpr std::string CmdLineFilename{"/cmdline"};
    constexpr std::string CpuInfo{"/cpuinfo"};
    constexpr std::string LoadAvgFilename{"/loadavg"};
    constexpr std::string StatusFile{"/status"};
    constexpr std::string StatFilename{"/stat"};
    constexpr std::string UptimeFilename{"/uptime"};
    constexpr std::string MemInfoFilename{"/meminfo"};
    constexpr std::string VersionFilename{"/version"};
    constexpr std::string OsPath{"/etc/os-release"};
    constexpr std::string PasswordPath{"/etc/passwd"};
    constexpr std::string NetDevFilename{"/net/dev"};
    constexpr std::string DiskStatsFilename{"/diskstats"};

    constexpr std::string FilterProcesses{"processes"};
    constexpr std::string FilterRunningProcesses{"procs_running"};
    constexpr std::string FilterMemTotal{"MemTotal:"};
    constexpr std::string FilterMemFree{"MemFree:"};
    constexpr std::string FilterMemAvailable{"MemAvailable:"};
    constexpr std::string FilterCPU{"cpu"};
    constexpr std::string FilterCpuCores{"cpu cores:"};
    constexpr std::string FilterUID{"Uid:"};
    constexpr std::string FilterProcMem{"VmData:"};
}