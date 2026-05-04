#include "services/agent_client.hpp"

#include <crow/http_response.h>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json_fwd.hpp>
#include <string>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}


AgentClient::AgentClient(const nlohmann::json &config) : config_(config) {}


nlohmann::json AgentClient::execute_command(const std::string &node_ip,
                                            const std::string &command) {
    std::cout << "[AgentClient] Executing on " << node_ip << ": " << command << std::endl;

    CURL *curl = curl_easy_init();
    if (!curl) return nlohmann::json::object();

    std::string url = "http://" + node_ip + ":8081/api/execute";
    nlohmann::json payload;
    payload["command"] = command;
    std::string payload_str = payload.dump();

    std::string response_string;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    CURLcode res = curl_easy_perform(curl);

    nlohmann::json data;
    if (res == CURLE_OK) {
      try {
            data = nlohmann::json::parse(response_string);
            std::cout << data << "\n";
            std::cout << "Command executed successfully\n";
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            // fallback: wrap raw text in JSON
            data = { {"output", response_string} };
        }
    } else {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        data = { {"error", curl_easy_strerror(res)} };
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return data;
}
