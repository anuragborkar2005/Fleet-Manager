#pragma once
#include <string>

class Disk
{
public:
    Disk(const std::string &device, const std::string &mountPoint);
    void updateIO();
    void updateFS();

    long diskReadsTotal() const;
    long diskWritesTotal() const;
    long diskReadBytesTotal() const;
    long diskWriteBytesTotal() const;
    double diskReadTimeSecondsTotal() const;
    double diskWriteTimeSecondsTotal() const;
    double diskIoTimeSecondsTotal() const;
    long diskIopsInProgress() const;

    long fsSizeBytes() const;
    long fsFreeBytes() const;
    long fsAvailBytes() const;
    long fsUsedBytes() const;
    double fsPercentUsed() const;

private:
    std::string device_;
    std::string mountPoint_;

    long readsCompleted_ = 0;
    long writesCompleted_ = 0;
    long sectorsRead_ = 0;
    long sectorsWritten_ = 0;
    long readTimeMs_ = 0;
    long writeTimeMs_ = 0;
    long ioTimeMs_ = 0;
    long iopsInProgress_ = 0;

    long sizeBytes_ = 0;
    long freeBytes_ = 0;
    long availBytes_ = 0;
    long usedBytes_ = 0;
};