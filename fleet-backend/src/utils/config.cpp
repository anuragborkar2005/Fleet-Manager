#include "utils/config.hpp"
#include <fstream>
#include <iostream>

json load_config_file(const std::string &file_name) {
  std::ifstream cfg_file(file_name);
  if (!cfg_file.is_open()) {
      std::cerr << "[Config] Error: Could not open " << file_name << "\n";
      return json::object();
  }
  try {
    json cfg = json::parse(cfg_file);
    return cfg;
  } catch (const std::exception& e) {
    std::cerr << "[Config] Error parsing JSON: " << e.what() << "\n";
    return json::object();
  }
}
