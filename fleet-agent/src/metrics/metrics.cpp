#include "metrics/metrics.hpp"
#include <chrono>
#include <iostream>

using namespace prometheus;

Metrics::Metrics(int port)
    : exposer{"0.0.0.0:" + std::to_string(port)},
      registry{std::make_shared<Registry>()}
{
    exposer.RegisterCollectable(registry);
}

Metrics::~Metrics()
{
    running = false;
    if (update_thread.joinable())
    {
        update_thread.join();
    }
}

void Metrics::start()
{

    auto &counter_family = BuildCounter()
                               .Name("heartbeat_total")
                               .Help("Number of heartbeats sent")
                               .Register(*registry);
    heartbeat_counter = &counter_family.Add({{"component", "agent"}});

    auto &cpu_gauge_family = BuildGauge()
                                 .Name("node_cpu_utilization")
                                 .Help("CPU utilization as a percentage")
                                 .Register(*registry);
    cpu_usage_gauge = &cpu_gauge_family.Add({});

    auto &core_gauge_family = BuildGauge()
                                  .Name("node_cpu_core_utilization")
                                  .Help("Per-core CPU utilization as a percentage")
                                  .Register(*registry);

    int cores = system.TotalCpuCores();
    for (int i = 0; i < cores; ++i)
    {
        core_usage_gauges.push_back(&core_gauge_family.Add({{"core", std::to_string(i)}}));
    }

    auto &cpu_time_family = BuildCounter()
                                .Name("node_cpu_seconds_total")
                                .Help("Total CPU time in jiffies")
                                .Register(*registry);
    cpu_total_time_counter = &cpu_time_family.Add({{"mode", "total"}});
    cpu_idle_time_counter = &cpu_time_family.Add({{"mode", "idle"}});

    auto &mem_gauge_family = BuildGauge()
                                 .Name("node_memory_utilization")
                                 .Help("Memory utilization as a percentage")
                                 .Register(*registry);
    memory_usage_gauge = &mem_gauge_family.Add({});

    auto &mem_total_family = BuildGauge()
                                 .Name("node_memory_total_bytes")
                                 .Help("Total physical memory in bytes")
                                 .Register(*registry);
    mem_total_gauge = &mem_total_family.Add({});

    auto &mem_free_family = BuildGauge()
                                .Name("node_memory_free_bytes")
                                .Help("Free physical memory in bytes")
                                .Register(*registry);
    mem_free_gauge = &mem_free_family.Add({});

    auto &mem_avail_family = BuildGauge()
                                 .Name("node_memory_available_bytes")
                                 .Help("Available physical memory in bytes")
                                 .Register(*registry);
    mem_available_gauge = &mem_avail_family.Add({});

    auto &mem_buffers_family = BuildGauge()
                                   .Name("node_memory_buffers_bytes")
                                   .Help("Memory used by kernel buffers in bytes")
                                   .Register(*registry);
    mem_buffers_gauge = &mem_buffers_family.Add({});

    auto &mem_cached_family = BuildGauge()
                                  .Name("node_memory_cached_bytes")
                                  .Help("Memory used by the page cache in bytes")
                                  .Register(*registry);
    mem_cached_gauge = &mem_cached_family.Add({});

    auto &swap_total_family = BuildGauge()
                                  .Name("node_memory_swap_total_bytes")
                                  .Help("Total swap space in bytes")
                                  .Register(*registry);
    swap_total_gauge = &swap_total_family.Add({});

    auto &swap_free_family = BuildGauge()
                                 .Name("node_memory_swap_free_bytes")
                                 .Help("Free swap space in bytes")
                                 .Register(*registry);
    swap_free_gauge = &swap_free_family.Add({});

    auto &total_proc_gauge_family = BuildGauge()
                                        .Name("node_total_processes")
                                        .Help("Total number of processes")
                                        .Register(*registry);
    total_processes_gauge = &total_proc_gauge_family.Add({});

    auto &running_proc_gauge_family = BuildGauge()
                                          .Name("node_running_processes")
                                          .Help("Number of running processes")
                                          .Register(*registry);
    running_processes_gauge = &running_proc_gauge_family.Add({});

    auto &uptime_gauge_family = BuildGauge()
                                    .Name("node_uptime_seconds")
                                    .Help("System uptime in seconds")
                                    .Register(*registry);
    uptime_gauge = &uptime_gauge_family.Add({});

    // Load Average Gauges
    auto &load1_family = BuildGauge()
                             .Name("node_load1")
                             .Help("System load average over 1 minute")
                             .Register(*registry);
    load1_gauge = &load1_family.Add({});

    auto &load5_family = BuildGauge()
                             .Name("node_load5")
                             .Help("System load average over 5 minutes")
                             .Register(*registry);
    load5_gauge = &load5_family.Add({});

    auto &load15_family = BuildGauge()
                              .Name("node_load15")
                              .Help("System load average over 15 minutes")
                              .Register(*registry);
    load15_gauge = &load15_family.Add({});

    // Disk Gauges
    auto &fs_usage_family = BuildGauge()
                                .Name("node_filesystem_usage_percentage")
                                .Help("Disk usage percentage")
                                .Register(*registry);
    fs_percent_used_gauge = &fs_usage_family.Add({});

    auto &fs_total_family = BuildGauge()
                                .Name("node_filesystem_size_bytes")
                                .Help("Total filesystem size in bytes")
                                .Register(*registry);
    fs_total_bytes_gauge = &fs_total_family.Add({});

    auto &fs_used_family = BuildGauge()
                               .Name("node_filesystem_used_bytes")
                               .Help("Used filesystem space in bytes")
                               .Register(*registry);
    fs_used_bytes_gauge = &fs_used_family.Add({});

    auto &disk_reads_family = BuildGauge()
                                  .Name("node_disk_reads_completed_total")
                                  .Help("Total number of disk reads completed")
                                  .Register(*registry);
    disk_reads_completed_gauge = &disk_reads_family.Add({});

    auto &disk_writes_family = BuildGauge()
                                   .Name("node_disk_writes_completed_total")
                                   .Help("Total number of disk writes completed")
                                   .Register(*registry);
    disk_writes_completed_gauge = &disk_writes_family.Add({});

    auto &disk_read_family = BuildGauge()
                                 .Name("node_disk_read_bytes_total")
                                 .Help("Total bytes read from disk")
                                 .Register(*registry);
    disk_read_bytes_gauge = &disk_read_family.Add({});

    auto &disk_write_family = BuildGauge()
                                  .Name("node_disk_write_bytes_total")
                                  .Help("Total bytes written to disk")
                                  .Register(*registry);
    disk_write_bytes_gauge = &disk_write_family.Add({});

    // Network Gauges
    auto &network_rx_family = BuildGauge()
                                  .Name("node_network_receive_bytes_total")
                                  .Help("Total bytes received")
                                  .Register(*registry);
    network_rx_bytes_gauge = &network_rx_family.Add({});

    auto &network_tx_family = BuildGauge()
                                  .Name("node_network_transmit_bytes_total")
                                  .Help("Total bytes transmitted")
                                  .Register(*registry);
    network_tx_bytes_gauge = &network_tx_family.Add({});

    auto &network_rx_pkt_family = BuildGauge()
                                      .Name("node_network_receive_packets_total")
                                      .Help("Total packets received")
                                      .Register(*registry);
    network_rx_packets_gauge = &network_rx_pkt_family.Add({});

    auto &network_tx_pkt_family = BuildGauge()
                                      .Name("node_network_transmit_packets_total")
                                      .Help("Total packets transmitted")
                                      .Register(*registry);
    network_tx_packets_gauge = &network_tx_pkt_family.Add({});

    auto &network_rx_err_family = BuildGauge()
                                      .Name("node_network_receive_errors_total")
                                      .Help("Total receive errors")
                                      .Register(*registry);
    network_rx_errors_gauge = &network_rx_err_family.Add({});

    auto &network_tx_err_family = BuildGauge()
                                      .Name("node_network_transmit_errors_total")
                                      .Help("Total transmit errors")
                                      .Register(*registry);
    network_tx_errors_gauge = &network_tx_err_family.Add({});

    auto &network_rx_drop_family = BuildGauge()
                                       .Name("node_network_receive_drops_total")
                                       .Help("Total receive packets dropped")
                                       .Register(*registry);
    network_rx_drops_gauge = &network_rx_drop_family.Add({});

    auto &network_tx_drop_family = BuildGauge()
                                       .Name("node_network_transmit_drops_total")
                                       .Help("Total transmit packets dropped")
                                       .Register(*registry);
    network_tx_drops_gauge = &network_tx_drop_family.Add({});

    update_thread = std::thread(&Metrics::update_loop, this);
}

void Metrics::update_loop()
{
    while (running)
    {
        try
        {
            // Per-core and Aggregate utilization
            auto cpu_util = system.CpuUtilization();
            
            if (!cpu_util.empty())
            {
                // First element is aggregate "cpu"
                cpu_usage_gauge->Set(cpu_util[0]);

                // Remaining elements are per-core "cpu0", "cpu1", ...
                for (size_t i = 1; i < cpu_util.size() && (i-1) < core_usage_gauges.size(); ++i)
                {
                    core_usage_gauges[i-1]->Set(cpu_util[i]);
                }
            }

            // CPU Stats (Counters)
            auto cpu_stats = system.CpuStats();
            if (cpu_stats.size() >= 2)
            {
                static long last_total = 0;
                static long last_idle = 0;

                if (last_total > 0)
                {
                    cpu_total_time_counter->Increment(static_cast<double>(cpu_stats[0] - last_total));
                    cpu_idle_time_counter->Increment(static_cast<double>(cpu_stats[1] - last_idle));
                }
                last_total = cpu_stats[0];
                last_idle = cpu_stats[1];
            }

            memory_usage_gauge->Set(system.MemoryUtilization());

            auto memInfo = system.DetailedMemory();
            mem_total_gauge->Set(memInfo.total);
            mem_free_gauge->Set(memInfo.free);
            mem_available_gauge->Set(memInfo.available);
            mem_buffers_gauge->Set(memInfo.buffers);
            mem_cached_gauge->Set(memInfo.cached);
            swap_total_gauge->Set(memInfo.swap_total);
            swap_free_gauge->Set(memInfo.swap_free);

            total_processes_gauge->Set(system.TotalProcesses());
            running_processes_gauge->Set(system.RunningProcesses());
            uptime_gauge->Set(system.UpTime());

            // Load Average Updates
            auto [l1, l5, l15, run, tot, last] = system.LoadAvg();
            load1_gauge->Set(l1);
            load5_gauge->Set(l5);
            load15_gauge->Set(l15);

            // Disk Updates
            disk.updateIO();
            disk.updateFS();
            fs_percent_used_gauge->Set(disk.fsPercentUsed());
            fs_total_bytes_gauge->Set(disk.fsSizeBytes());
            fs_used_bytes_gauge->Set(disk.fsUsedBytes());
            disk_reads_completed_gauge->Set(disk.diskReadsTotal());
            disk_writes_completed_gauge->Set(disk.diskWritesTotal());
            disk_read_bytes_gauge->Set(disk.diskReadBytesTotal());
            disk_write_bytes_gauge->Set(disk.diskWriteBytesTotal());

            // Network Updates
            network.update();
            network_rx_bytes_gauge->Set(network.receiveBytesTotal());
            network_tx_bytes_gauge->Set(network.transmitBytesTotal());
            network_rx_packets_gauge->Set(network.receivePacketsTotal());
            network_tx_packets_gauge->Set(network.transmitPacketsTotal());
            network_rx_errors_gauge->Set(network.receiveErrorsTotal());
            network_tx_errors_gauge->Set(network.transmitErrorsTotal());
            network_rx_drops_gauge->Set(network.receiveDropsTotal());
            network_tx_drops_gauge->Set(network.transmitDropsTotal());
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Metrics] Error updating system metrics: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Metrics::incrementHeartbeat()
{
    if (heartbeat_counter)
    {
        heartbeat_counter->Increment();
    }
}
