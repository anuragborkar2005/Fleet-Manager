#pragma once

#include<nlohmann/json.hpp>
#include <string>

class AgentClient {
private:
    nlohmann::json config_;

public:
  explicit AgentClient(const nlohmann::json &config);
  nlohmann::json execute_command(const std::string& node_ip, const std::string& command);
};
