#pragma once

#include "system.hpp"

#include <atomic>
#include <thread>

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/registry.h>

using namespace prometheus;

class Metrics
{
private:
    Exposer exposer;
    std::shared_ptr<Registry> registry;

    System system;

    Counter *heartbeat_counter;
    Gauge *cpu_usage_gauge;
    Gauge *memory_usage_gauge;
    Gauge *total_processes_gauge;
    Gauge *running_processes_gauge;
    Gauge *uptime_gauge;

    std::atomic<bool> running{true};
    std::thread update_thread;

    void update_loop();

public:
    Metrics(int port);
    ~Metrics();
    void start();
    void increamentHeartbeat();
};