#pragma once

#include <string>
#include <vector>

class Process
{
private:
    int pids_;
    long Hertz_;
    float utime_ = 0.0;
    float stime_ = 0.0;
    float cutime_ = 0.0;
    float cstime_ = 0.0;
    float starttime_ = 0.0;

    std::vector<std::string> ReadFile(int);

public:
    Process(int, long);
    int Pid();
    std::string User();
    std::string Command();
    double CpuUtilization();
    float RawRam();
    std::string Ram();
    long int UpTime();
};