#include "processor.hpp"
#include "parser_constants.hpp"

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

std::vector<double> Processor::ReadFile()
{
    std::string line, key;
    double value;
    std::vector<double> cpuNumbers;
    std::ifstream stream(ParserConstants::ProcDirectory + ParserConstants::StatFilename);
    if (stream.is_open())
    {
        while (std::getline(stream, line))
        {
            std::istringstream linestream(line);
            while (linestream >> key)
            {
                if (key == ParserConstants::FilterCPU)
                {
                    while (linestream >> value)
                    {
                        cpuNumbers.emplace_back(value);
                    }
                }
            }
        }
    }
    return cpuNumbers;
}

double Processor::Utilization()
{
    std::vector<double> values = ReadFile();
    double user = values[0];
    double nice = values[1];
    double system = values[2];
    double idle = values[3];
    double iowait = values[4];
    double irq = values[5];
    double softirq = values[6];
    double steal = values[7];

    double PrevIdle = previdle + previowait;
    double Idle = idle + iowait;

    double PrevNonIdle =
        prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    double NonIdle = user + nice + system + irq + softirq + steal;

    double PrevTotal = PrevIdle + PrevNonIdle;
    double Total = Idle + NonIdle;

    double totald = Total - PrevTotal;

    double idled = Idle - PrevIdle;

    double CPU_Percentage = (totald - idled) / totald;

    AssignPrevValues(values);
    return CPU_Percentage;
}

void Processor::AssignPrevValues(std::vector<double> newValues)
{
    prevuser = newValues[0];
    prevnice = newValues[1];
    prevsystem = newValues[2];
    previdle = newValues[3];
    previowait = newValues[4];
    previrq = newValues[5];
    prevsoftirq = newValues[6];
    prevsteal = newValues[7];
}