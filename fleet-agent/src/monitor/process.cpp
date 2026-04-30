#include "utils/format.hpp"
#include "utils/parser_constants.hpp"
#include "utils/parser_helper.hpp"
#include "utils/system_parser.hpp"
#include "monitor/process.hpp"

#include <iterator>
#include <fstream>
#include <sstream>


std::vector<std::string> Process::ReadFile(int pid) {
    std::ifstream filestream(ParserConstants::ProcDirectory + std::to_string(pid) +
                             ParserConstants::StatFilename);
    if (!filestream.is_open()) {
        return {};
    }

    std::string line;
    std::getline(filestream, line);

    auto lparen = line.find('(');
    auto rparen = line.rfind(')');
    if (lparen == std::string::npos || rparen == std::string::npos) {
        return {};
    }

    std::string pid_str = line.substr(0, lparen - 1);
    std::string comm    = line.substr(lparen + 1, rparen - lparen - 1);

    std::istringstream iss(line.substr(rparen + 2));
    std::vector<std::string> fields{std::istream_iterator<std::string>{iss}, {}};

    fields.insert(fields.begin(), comm);
    fields.insert(fields.begin(), pid_str);

    return fields;
}


Process::Process(int pid, long Hertz) : pid_(pid), Hertz_(Hertz) {
    auto cpuNumbers = ReadFile(pid);
    if (cpuNumbers.size() <= 21) {
        utime_ = stime_ = cutime_ = cstime_ = starttime_ = 0;
        return;
    }
    utime_     = std::stof(cpuNumbers[13]);
    stime_     = std::stof(cpuNumbers[14]);
    cutime_    = std::stof(cpuNumbers[15]);
    cstime_    = std::stof(cpuNumbers[16]);
    starttime_ = std::stof(cpuNumbers[21]);
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
