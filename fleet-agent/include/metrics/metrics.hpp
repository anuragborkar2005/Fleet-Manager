#pragma once

#include "monitor/system.hpp"
#include "monitor/disk.hpp"
#include "monitor/network.hpp"

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
    Disk disk{"sda", "/"};
    Network network{"enp0s3"};

    Counter *heartbeat_counter;
    Gauge *cpu_usage_gauge;
    std::vector<Gauge *> core_usage_gauges;
    Counter *cpu_total_time_counter;
    Counter *cpu_idle_time_counter;
    Gauge *memory_usage_gauge;
    Gauge *mem_total_gauge;
    Gauge *mem_free_gauge;
    Gauge *mem_available_gauge;
    Gauge *mem_buffers_gauge;
    Gauge *mem_cached_gauge;
    Gauge *swap_total_gauge;
    Gauge *swap_free_gauge;
    Gauge *total_processes_gauge;
    Gauge *running_processes_gauge;
    Gauge *uptime_gauge;

    // Load Average Metrics
    Gauge *load1_gauge;
    Gauge *load5_gauge;
    Gauge *load15_gauge;

    // Disk Metrics
    Gauge *fs_percent_used_gauge;
    Gauge *fs_total_bytes_gauge;
    Gauge *fs_used_bytes_gauge;
    Gauge *disk_reads_completed_gauge;
    Gauge *disk_writes_completed_gauge;
    Gauge *disk_read_bytes_gauge;
    Gauge *disk_write_bytes_gauge;

    // Network Metrics
    Gauge *network_rx_bytes_gauge;
    Gauge *network_tx_bytes_gauge;
    Gauge *network_rx_packets_gauge;
    Gauge *network_tx_packets_gauge;
    Gauge *network_rx_errors_gauge;
    Gauge *network_tx_errors_gauge;
    Gauge *network_rx_drops_gauge;
    Gauge *network_tx_drops_gauge;

    std::atomic<bool> running{true};
    std::thread update_thread;

    void update_loop();

public:
    Metrics(int port);
    ~Metrics();
    void start();
    void incrementHeartbeat();
};
