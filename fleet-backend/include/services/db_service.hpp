#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json_fwd.hpp>
#include <vector>

class DBService {
private:
  std::unique_ptr<SQLite::Database> db_;
  nlohmann::json config_;
public:
  DBService(const nlohmann::json &);

  void init();
  void register_node(const nlohmann::json &);
  void update_heartbeat(const std::string &);
  std::vector<nlohmann::json> get_active_node();
  void log_command(const std::string &node_id, const std::string &command,
                   int exit_code, const std::string &stdout_str,
                   const std::string &stderr_str);
};
