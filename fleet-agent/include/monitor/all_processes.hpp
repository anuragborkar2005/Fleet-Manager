#pragma once

#include "process.hpp"

class AllProcesses
{
private:
    long Hertz_;
    std::vector<int> current_pids_;
    std::vector<Process> all_processes_;

    void UpdateProcesses();
    void AddNewProcesses(bool &);
    std::vector<int> ReadFolders();
    void RemoveFinishedProcesses(bool &);

public:
    AllProcesses();
    std::vector<Process> &GetProcesses();
};