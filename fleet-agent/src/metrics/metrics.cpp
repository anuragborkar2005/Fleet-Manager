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

    auto &mem_gauge_family = BuildGauge()
                                 .Name("node_memory_utilization")
                                 .Help("Memory utilization as a percentage")
                                 .Register(*registry);
    memory_usage_gauge = &mem_gauge_family.Add({});

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

    update_thread = std::thread(&Metrics::update_loop, this);
}

void Metrics::update_loop()
{
    while (running)
    {
        try
        {
            cpu_usage_gauge->Set(system.Cpu().Utilization());
            memory_usage_gauge->Set(system.MemoryUtilization());
            total_processes_gauge->Set(system.TotalProcesses());
            running_processes_gauge->Set(system.RunningProcesses());
            uptime_gauge->Set(system.UpTime());
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
