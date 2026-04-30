#include "agent.hpp"
#include "config.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <ifaddrs.h>
#include <curl/curl.h>

Agent::Agent()
{
    load_config();
    metrics = std::make_unique<Metrics>(metrics_port);
    metrics->start();
}

void Agent::load_config()
{
    auto cfg = load_config_file("config.json");
    if (cfg.empty())
    {
        std::cerr << "[Agent] Error: config.json not found or empty. Using defaults.\n";
        server = "http://localhost:8080";
        heartbeat_interval = 30;
        port = 8080;
        metrics_port = 8082;
        return;
    }

    try
    {
        server = cfg.value("server", "http://localhost:8080");
        heartbeat_interval = cfg.value("heartbeat_interval_seconds", 30);

        if (cfg.contains("agent_port"))
        {
            std::string port_str = cfg["agent_port"].get<std::string>();
            port = std::stoi(port_str);
        }
        else
        {
            port = 8080;
        }

        if (cfg.contains("metrics_port"))
        {
            std::string m_port_str = cfg["metrics_port"].get<std::string>();
            metrics_port = std::stoi(m_port_str);
        }
        else
        {
            metrics_port = 8082;
        }

        std::cout << "[Agent] Config loaded. Server: " << server << ", Agent Port: " << port << ", Metrics Port: " << metrics_port << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Agent] Error parsing config: " << e.what() << ". Using defaults.\n";
        server = "http://localhost:8080";
        heartbeat_interval = 30;
        port = 8080;
        metrics_port = 8082;
    }
}

int Agent::get_port() const
{
    return port;
}

int Agent::get_metrics_port() const
{
    return metrics_port;
}

std::string Agent::get_node_info()
{
    json cfg;
    char hostname[255];
    gethostname(hostname, sizeof(hostname));
    cfg["hostname"] = hostname;

    struct ifaddrs *ifaddr;
    getifaddrs(&ifaddr);

    for (auto ifa = ifaddr; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET &&
            (std::string(ifa->ifa_name) == "eth0" ||
             std::string(ifa->ifa_name) == "enp0s3"))
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip,
                      sizeof(ip));
            cfg["ip"] = ip;
            break;
        }
    }

    freeifaddrs(ifaddr);
    cfg["os"] = "linux";
    cfg["agent_version"] = "1.0.0";
    return cfg.dump();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void Agent::register_node()
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return;

    std::string url = server + "/api/node/register";
    std::string payload = get_node_info();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
        auto data = json::parse(response);
        std::cout << "[Agent] Registered Successfully\n";
    }
    else
    {
        std::cerr << "Curl failed: " << curl_easy_strerror(res) << "\n";
    }
    curl_easy_cleanup(curl);
}

void Agent::send_heartbeat()
{
    while (active)
    {
        std::this_thread::sleep_for(std::chrono::seconds(heartbeat_interval));

        CURL *curl = curl_easy_init();
        if (!curl)
            return;
        std::string url = server + "/api/nodes/heartbeat";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            std::cout << "Heartbeat sent\n";
        }
    }
}

void Agent::start_registration_and_heartbeat()
{
    register_node();
    std::thread heartbeat_thread(&Agent::send_heartbeat, this);
    heartbeat_thread.detach();
}

void Agent::set_token(const std::string &token)
{
    std::lock_guard<std::mutex> lock(mtx);
    jwt_token = token;
}

std::string Agent::get_jwt() const
{
    return jwt_token;
}