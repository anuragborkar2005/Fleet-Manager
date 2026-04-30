#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json load_config_file(const std::string &filename);
