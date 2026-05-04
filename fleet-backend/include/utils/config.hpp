#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
json load_config_file(const std::string &file_name);
