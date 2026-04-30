#include "utils/format.hpp"
#include "utils/parser_constants.hpp"
#include "utils/parser_helper.hpp"
#include "utils/system_parser.hpp"
#include "monitor/process.hpp"

#include <iterator>
#include <fstream>
#include <sstream>

Process::Process(int pid, long Hertz) : pid_(pid), Hertz_(Hertz)
{
    std::vector<std::string> cpuNumbers = ReadFile(pid);

    utime_ = stof(cpuNumbers[13]);
    stime_ = stof(cpuNumbers[14]);
    cutime_ = stof(cpuNumbers[15]);
    cstime_ = stof(cpuNumbers[16]);
    starttime_ = stof(cpuNumbers[21]);
}

int Process::Pid() { return pid_; }

double Process::CpuUtilization()
{
    long uptime = SystemParser::UpTime();
    double total_time = utime_ + stime_ + cutime_ + cstime_;

    double seconds = uptime - (starttime_ / Hertz_);
    double cpu_usage = (total_time / Hertz_) / seconds;

    return cpu_usage;
}

std::string Process::Command()
{
    std::string cmd = ParserHelper::GetValue<std::string>(std::to_string(pid_) +
                                                          ParserConstants::CmdLineFilename);
    size_t maxSize = 50;
    if (cmd.size() > maxSize)
    {
        cmd.resize(maxSize - 3);
        cmd = cmd + "...";
    }
    return cmd;
}

float Process::RawRam()
{
    float memInKB = ParserHelper::GetValueByKey<float>(
        ParserConstants::FilterProcMem,
        std::to_string(pid_) + ParserConstants::StatusFile);
    return memInKB;
}

std::string Process::User()
{
    int UID = ParserHelper::GetValueByKey<int>(
        ParserConstants::FilterUID, std::to_string(pid_) + ParserConstants::StatusFile);

    std::string user = SystemParser::UserByID(UID);
    return user;
}

std::string Process::Ram()
{
    float memInKB = RawRam();
    return Format::KBtoMB(memInKB);
}

long int Process::UpTime()
{
    long uptime = SystemParser::UpTime();
    long seconds = uptime - (starttime_ / Hertz_);

    return seconds;
}

std::vector<std::string> Process::ReadFile(int pid)
{
    std::string line, skip;

    std::ifstream filestream(ParserConstants::ProcDirectory + std::to_string(pid) +
                             ParserConstants::StatFilename);

    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<std::string> beg(linestream), end;
    std::vector<std::string> cpuNumbers(beg, end);
    return cpuNumbers;
};
