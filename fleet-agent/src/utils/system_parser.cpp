#include "system_parser.hpp"
#include "parser_constants.hpp"
#include "parser_helper.hpp"

#include <algorithm>
#include <fstream>
#include <filesystem>
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
                    std::replace(value.begin(), value.end(), "_", " ");
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

float SystemParser::MemoryUtilization()
{
    float memTotal = ParserHelper::GetValueByKey<int>(ParserConstants::FilterMemTotal, ParserConstants::MemInfoFilename);
    float memFree = ParserHelper::GetValueByKey<int>(ParserConstants::FilterMemFree, ParserConstants::MemInfoFilename);
    float memory = (memTotal - memFree) / memTotal;
    return memory;
}

std::vector<int> SystemParser::Pids()
{
    std::vector<int> pids;
    for (const auto &entry : std::filesystem::directory_iterator(ParserConstants::ProcDirectory))
    {
        if (entry.is_directory())
        {
            std::string filename = entry.path().filename().string();
            if (std::all_of(filename.begin(), filename.end(), isdigit))
            {
                int pid = std::stoi(filename);
                pids.push_back(pid);
            }
        }
    }
    return pids;
};

int SystemParser::TotalProcesses()
{
    return ParserHelper::GetValueByKey<int>(ParserConstants::FilterProcesses, ParserConstants::StatFilename);
}

int SystemParser::RunningProcesses()
{
    return ParserHelper::GetValueByKey<int>(ParserConstants::FilterRuningProcesses, ParserConstants::StatFilename);
}
