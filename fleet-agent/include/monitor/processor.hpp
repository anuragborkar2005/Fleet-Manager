#pragma once

#include <vector>

class Processor
{
private:
    int previdle;
    int previowait;
    int prevuser;
    int prevnice;
    int prevsystem;
    int previrq;
    int prevsoftirq;
    int prevsteal;

    void AssignPrevValues(std::vector<double>);
    std::vector<double> ReadFile();

public:
    double Utilization();
};