#pragma once
#include "metrics.hpp"

#include <atomic>
#include <mutex>
#include <string>
#include <memory>

class Agent
{
private:
    std::string jwt_token;
    std::string server;
    int port;
    int metrics_port;
    int heartbeat_interval;
    std::atomic<bool> active{true};
    std::mutex mtx;

    void register_node();
    void send_heartbeat();
    void load_config();
    std::string get_node_info();

public:
    Agent();
    std::unique_ptr<Metrics> metrics;
    void start_registration_and_heartbeat();
    std::string get_jwt() const;
    void set_token(const std::string &token);
    int get_port() const;
    int get_metrics_port() const;
};