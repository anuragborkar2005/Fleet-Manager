#include "monitor/disk.hpp"
#include <fstream>
#include <sstream>
#include <sys/statvfs.h>
#include <iostream>

Disk::Disk(const std::string &device, const std::string &mountPoint)
    : device_(device), mountPoint_(mountPoint)
{
    updateIO();
    updateFS();
}

void Disk::updateIO()
{
    std::ifstream file("/proc/diskstats");
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        int major, minor;
        std::string name;
        long reads, readsMerged, sectorsRead, readMs;
        long writes, writesMerged, sectorsWritten, writeMs;
        long ioInProgress, ioMs, weightedIoMs;

        if (iss >> major >> minor >> name >> reads >> readsMerged >> sectorsRead >> readMs >> writes >> writesMerged >> sectorsWritten >> writeMs >> ioInProgress >> ioMs >> weightedIoMs)
        {
            if (name == device_)
            {
                readsCompleted_ = reads;
                writesCompleted_ = writes;
                sectorsRead_ = sectorsRead;
                sectorsWritten_ = sectorsWritten;
                readTimeMs_ = readMs;
                writeTimeMs_ = writeMs;
                ioTimeMs_ = ioMs;
                iopsInProgress_ = ioInProgress;
                break;
            }
        }
    }
}

void Disk::updateFS()
{
    struct statvfs stat;
    if (statvfs(mountPoint_.c_str(), &stat) == 0)
    {
        sizeBytes_ = stat.f_blocks * stat.f_frsize;
        freeBytes_ = stat.f_bfree * stat.f_frsize;
        availBytes_ = stat.f_bavail * stat.f_frsize;
        usedBytes_ = sizeBytes_ - freeBytes_;
    }
}

long Disk::diskReadsTotal() const { return readsCompleted_; }
long Disk::diskWritesTotal() const { return writesCompleted_; }
long Disk::diskReadBytesTotal() const { return sectorsRead_ * 512; }
long Disk::diskWriteBytesTotal() const { return sectorsWritten_ * 512; }
double Disk::diskReadTimeSecondsTotal() const { return readTimeMs_ / 1000.0; }
double Disk::diskWriteTimeSecondsTotal() const { return writeTimeMs_ / 1000.0; }
double Disk::diskIoTimeSecondsTotal() const { return ioTimeMs_ / 1000.0; }
long Disk::diskIopsInProgress() const { return iopsInProgress_; }

long Disk::fsSizeBytes() const { return sizeBytes_; }
long Disk::fsFreeBytes() const { return freeBytes_; }
long Disk::fsAvailBytes() const { return availBytes_; }
long Disk::fsUsedBytes() const { return usedBytes_; }

double Disk::fsPercentUsed() const
{
    if (sizeBytes_ == 0)
        return 0.0;
    return static_cast<double>(usedBytes_) / static_cast<double>(sizeBytes_) * 100.0;
}
