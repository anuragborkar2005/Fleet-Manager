#include "agent/agent.hpp"
#include "utils/config.hpp"

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
        cfg = load_config_file("../config.json");
    }

    server = "http://localhost:8080";
    heartbeat_interval = 30;
    port = 8080;
    metrics_port = 8082;
    secret = "";

    if (!cfg.empty())
    {
        try
        {
            server = cfg.value("server", server);
            heartbeat_interval = cfg.value("heartbeat_interval_seconds", heartbeat_interval);
            secret = cfg.value("secret", secret);

            if (cfg.contains("agent_port"))
            {
                auto val = cfg["agent_port"];
                if (val.is_string())
                {
                    port = std::stoi(val.get<std::string>());
                }
                else if (val.is_number())
                {
                    port = val.get<int>();
                }
            }

            if (cfg.contains("metrics_port"))
            {
                auto val = cfg["metrics_port"];
                if (val.is_string())
                {
                    metrics_port = std::stoi(val.get<std::string>());
                }
                else if (val.is_number())
                {
                    metrics_port = val.get<int>();
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Agent] Error parsing config: " << e.what() << ". Using defaults.\n";
        }
    }

    if (const char *env_server = std::getenv("FLEET_BACKEND_URL"))
    {
        server = env_server;
        std::cout << "[Agent] Using server URL from environment: " << server << "\n";
    }

    if (const char *env_port = std::getenv("FLEET_AGENT_PORT"))
    {
        port = std::stoi(env_port);
    }

    std::cout << "[Agent] Config loaded. Server: " << server << ", Agent Port: " << port << ", Metrics Port: " << metrics_port << "\n";
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
             std::string(ifa->ifa_name) == "enp0s3" ||
             std::string(ifa->ifa_name) == "lo"))
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip,
                      sizeof(ip));
            cfg["ip"] = ip;
            if (std::string(ifa->ifa_name) != "lo")
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
    bool registered = false;
    while (!registered && active)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        std::string url = server + "/api/nodes/register";
        std::string payload = get_node_info();
        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (!secret.empty())
        {
            std::string authHeader = "Authorization: Bearer " + secret;
            headers = curl_slist_append(headers, authHeader.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200)
            {
                try
                {
                    auto data = json::parse(response);
                    if (data.contains("jwt"))
                    {
                        set_token(data["jwt"]);
                    }
                    if (data.contains("node_id"))
                    {
                        node_id = data["node_id"].get<std::string>();
                        std::cout << "[Agent] Registered Successfully. Node ID: " << node_id << "\n";
                        registered = true;
                    }
                }
                catch (const json::parse_error &e)
                {
                    std::cerr << "[Agent] JSON Parse Error during registration: " << e.what() << "\n";
                }
            }
            else
            {
                std::cerr << "[Agent] Registration failed with HTTP code: " << http_code << " (URL: " << url << ")\n";
            }
        }
        else
        {
            std::cerr << "[Agent] Curl failed during registration: " << curl_easy_strerror(res) << " (URL: " << url << ")\n";
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (!registered)
        {
            std::cout << "[Agent] Retrying registration in 5 seconds...\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}

void Agent::send_heartbeat()
{
    while (active)
    {
        std::this_thread::sleep_for(std::chrono::seconds(heartbeat_interval));

        if (node_id.empty())
        {
            std::cerr << "[Agent] Skipping heartbeat: Node not registered yet\n";
            continue;
        }

        CURL *curl = curl_easy_init();
        if (!curl)
            continue;
        std::string url = server + "/api/nodes/heartbeat";

        json payload;
        payload["node_id"] = node_id;
        std::string payload_str = payload.dump();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());

        struct curl_slist *headers = NULL;

        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!jwt_token.empty())
        {
            std::string authHeader = "Authorization: Bearer " + jwt_token;
            headers = curl_slist_append(headers, authHeader.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200)
            {
                std::cout << "[Agent] Heartbeat sent for Node ID: " << node_id << "\n";
                metrics->incrementHeartbeat();
            }
            else
            {
                std::cerr << "[Agent] Heartbeat failed with HTTP code: " << http_code << "\n";
            }
        }
        else
        {
            std::cerr << "[Agent] Heartbeat Curl failed: " << curl_easy_strerror(res) << "\n";
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
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
