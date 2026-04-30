#include "monitor/all_processes.hpp"
#include "monitor/process.hpp"
#include "utils/system_parser.hpp"

#include <algorithm>
#include <unistd.h>

bool compare_processes(Process &p1, Process &p2)
{
    return (p1.RawRam() > p2.RawRam());
}

AllProcesses::AllProcesses()
{
    Hertz_ = sysconf(_SC_CLK_TCK);
    UpdateProcesses();
}

std::vector<int> AllProcesses::ReadFolders()
{
    return SystemParser::Pids();
}

void AllProcesses::AddNewProcesses(bool &changed)
{
    for (size_t i = 0; i < current_pids_.size(); ++i)
    {
        int current_pid = current_pids_[i];
        auto it = std::find_if(all_processes_.begin(), all_processes_.end(), [current_pid](Process &p)
                               { return p.Pid() == current_pid; });

        if (it == all_processes_.end())
        {
            changed = true;
            Process process{current_pid, Hertz_};
            all_processes_.emplace_back(process);
        }
    }
}

std::vector<Process> &AllProcesses::GetProcesses()
{
    UpdateProcesses();
    return all_processes_;
}

void AllProcesses::UpdateProcesses()
{
    current_pids_ = ReadFolders();

    bool changed = false;
    AddNewProcesses(changed);
    RemoveFinishedProcesses(changed);

    if (changed)
    {
        std::sort(all_processes_.begin(), all_processes_.end(), compare_processes);
    }
}

void AllProcesses::RemoveFinishedProcesses(bool &changed)
{
    for (size_t i = 0; i < all_processes_.size(); i++)
    {
        int current_pid = all_processes_[i].Pid();

        if (std::find(current_pids_.begin(), current_pids_.end(), current_pid) ==
            current_pids_.end())
        {
            changed = true;

            all_processes_.erase(all_processes_.begin() + i);
        }
    }
}
