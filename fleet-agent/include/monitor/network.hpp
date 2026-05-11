#pragma once
#include <string>

class Network
{
public:
    Network(const std::string &iface);

    void update();

    long receiveBytesTotal() const;
    long transmitBytesTotal() const;
    long receivePacketsTotal() const;
    long transmitPacketsTotal() const;
    long receiveErrorsTotal() const;
    long transmitErrorsTotal() const;
    long receiveDropsTotal() const;
    long transmitDropsTotal() const;

private:
    std::string iface_;

    long rxBytes_ = 0;
    long txBytes_ = 0;
    long rxPackets_ = 0;
    long txPackets_ = 0;
    long rxErrors_ = 0;
    long txErrors_ = 0;
    long rxDrops_ = 0;
    long txDrops_ = 0;
};
