#include "utils/system_parser.hpp"
#include "utils/parser_constants.hpp"
#include "utils/parser_helper.hpp"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

std::string SystemParser::OperatingSystem()
{
    std::string line, key, value;
    std::ifstream filestream(ParserConstants::OsPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value)
            {
                if (key == "PRETTY_NAME")
                {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

std::string SystemParser::Kernel()
{
    std::string line;
    std::string os, version, kernel;
    std::ifstream filestream(ParserConstants::ProcDirectory + ParserConstants::VersionFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> os >> version >> kernel;
            return kernel;
        }
    }
    return kernel;
}

std::string SystemParser::UserByID(int UID)
{
    std::string line, user, x;
    int fileUID;

    std::ifstream filestream(ParserConstants::PasswordPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            while (linestream >> user >> x >> fileUID)
            {
                if (fileUID == UID)
                {
                    return user;
                }
            }
        }
    }
    return user;
}

long SystemParser::UpTime()
{
    std::string line;
    long upTime = ParserHelper::GetValue<long>(ParserConstants::UptimeFilename);
    return upTime;
}

SystemParser::MemoryInfo SystemParser::DetailedMemory()
{
    MemoryInfo info{};
    info.total = ParserHelper::GetValueByKey<long>(ParserConstants::FilterMemTotal, ParserConstants::MemInfoFilename);
    info.free = ParserHelper::GetValueByKey<long>(ParserConstants::FilterMemFree, ParserConstants::MemInfoFilename);
    info.available = ParserHelper::GetValueByKey<long>(ParserConstants::FilterMemAvailable, ParserConstants::MemInfoFilename);
    info.buffers = ParserHelper::GetValueByKey<long>("Buffers:", ParserConstants::MemInfoFilename);
    info.cached = ParserHelper::GetValueByKey<long>("Cached:", ParserConstants::MemInfoFilename);
    info.swap_total = ParserHelper::GetValueByKey<long>("SwapTotal:", ParserConstants::MemInfoFilename);
    info.swap_free = ParserHelper::GetValueByKey<long>("SwapFree:", ParserConstants::MemInfoFilename);
    
    // Convert KB to Bytes
    info.total *= 1024;
    info.free *= 1024;
    info.available *= 1024;
    info.buffers *= 1024;
    info.cached *= 1024;
    info.swap_total *= 1024;
    info.swap_free *= 1024;

    return info;
}

float SystemParser::MemoryUtilization()
{
    float memTotal = ParserHelper::GetValueByKey<int>(ParserConstants::FilterMemTotal, ParserConstants::MemInfoFilename);
    float memAvailable = ParserHelper::GetValueByKey<int>(
        ParserConstants::FilterMemAvailable, ParserConstants::MemInfoFilename);

    if (memTotal == 0)
        return 0.0;
    return (memTotal - memAvailable) / memTotal;
}

int SystemParser::TotalProcesses()
{
    return ParserHelper::GetValueByKey<int>(ParserConstants::FilterProcesses, ParserConstants::StatFilename);
}

int SystemParser::RunningProcesses()
{
    return ParserHelper::GetValueByKey<int>(ParserConstants::FilterRunningProcesses, ParserConstants::StatFilename);
}

std::tuple<double, double, double, int, int, int> SystemParser::LoadAvg()
{
    std::string line, running_total;
    double load1 = 0.0, load5 = 0.0, load15 = 0.0;
    int running_tasks = 0, total_tasks = 0, last_pid = 0;

    std::ifstream file(ParserConstants::ProcDirectory + ParserConstants::LoadAvgFilename);

    if (file.is_open() && std::getline(file, line))
    {
        std::istringstream linestream(line);
        linestream >> load1 >> load5 >> load15 >> running_total >> last_pid;

        size_t slash_pos = running_total.find('/');
        if (slash_pos != std::string::npos)
        {
            running_tasks = std::stoi(running_total.substr(0, slash_pos));
            total_tasks = std::stoi(running_total.substr(slash_pos + 1));
        }
    }

    return {load1, load5, load15, running_tasks, total_tasks, last_pid};
}

int SystemParser::TotalCpuCores()
{
    int cpu_cores = ParserHelper::GetValueByKey<int>(ParserConstants::FilterCpuCores, ParserConstants::CpuInfo);
    return cpu_cores;
}

#include <map>

// Helper to store previous CPU stats for each core and aggregate
static std::map<std::string, std::pair<long, long>> prev_cpu_data;

std::vector<double> SystemParser::CpuUtilization()
{
    std::vector<double> utilization;
    std::ifstream filestream(ParserConstants::ProcDirectory + ParserConstants::StatFilename);
    std::string line;

    while (std::getline(filestream, line))
    {
        if (line.compare(0, 3, "cpu") == 0)
        {
            std::istringstream linestream(line);
            std::string cpuName;
            long user, nice, system, idle, iowait, irq, softirq, steal;
            linestream >> cpuName >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

            long total = user + nice + system + idle + iowait + irq + softirq + steal;
            long idle_time = idle + iowait;

            if (prev_cpu_data.count(cpuName))
            {
                long prevTotal = prev_cpu_data[cpuName].first;
                long prevIdle = prev_cpu_data[cpuName].second;

                long totalDiff = total - prevTotal;
                long idleDiff = idle_time - prevIdle;

                if (totalDiff > 0)
                {
                    utilization.push_back(static_cast<double>(totalDiff - idleDiff) / totalDiff);
                }
                else
                {
                    utilization.push_back(0.0);
                }
            }
            else
            {
                utilization.push_back(0.0);
            }

            prev_cpu_data[cpuName] = {total, idle_time};
        }
    }
    return utilization;
}

std::vector<long> SystemParser::CpuStats()
{
    std::string line;
    std::ifstream filestream(ParserConstants::ProcDirectory + ParserConstants::StatFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            if (line.compare(0, 3, "cpu") == 0 && !isdigit(line[3]))
            {
                std::istringstream linestream(line);
                std::string cpu;
                long user, nice, system, idle, iowait, irq, softirq, steal;
                linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

                long total = user + nice + system + idle + iowait + irq + softirq + steal;
                long idle_time = idle + iowait;
                long busy = total - idle_time;

                return {total, idle_time, busy};
            }
        }
    }
    return {};
}
