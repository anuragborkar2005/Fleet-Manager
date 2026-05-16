#include "monitor/network.hpp"
#include "utils/parser_constants.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Network::Network(const std::string &iface) : iface_(iface) {}

void Network::update()
{
    std::ifstream file(ParserConstants::ProcDirectory + ParserConstants::NetDevFilename);
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open " << ParserConstants::ProcDirectory + ParserConstants::NetDevFilename << "\n";
        return;
    }

    std::string line;
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string name;
        if (!(iss >> name))
            continue;

        if (name.back() == ':')
            name.pop_back();

        if (name == iface_)
        {
            long rxBytes, rxPackets, rxErrs, rxDrop, rxFifo, rxFrame, rxCompressed, rxMulticast;
            long txBytes, txPackets, txErrs, txDrop, txFifo, txColls, txCarrier, txCompressed;

            if (iss >> rxBytes >> rxPackets >> rxErrs >> rxDrop >> rxFifo >> rxFrame >> rxCompressed >> rxMulticast >> txBytes >> txPackets >> txErrs >> txDrop >> txFifo >> txColls >> txCarrier >> txCompressed)
            {
                rxBytes_ = rxBytes;
                rxPackets_ = rxPackets;
                rxErrors_ = rxErrs;
                rxDrops_ = rxDrop;
                txBytes_ = txBytes;
                txPackets_ = txPackets;
                txErrors_ = txErrs;
                txDrops_ = txDrop;
            }
            break;
        }
    }
}

// Getters
long Network::receiveBytesTotal() const { return rxBytes_; }
long Network::transmitBytesTotal() const { return txBytes_; }
long Network::receivePacketsTotal() const { return rxPackets_; }
long Network::transmitPacketsTotal() const { return txPackets_; }
long Network::receiveErrorsTotal() const { return rxErrors_; }
long Network::transmitErrorsTotal() const { return txErrors_; }
long Network::receiveDropsTotal() const { return rxDrops_; }
long Network::transmitDropsTotal() const { return txDrops_; }
