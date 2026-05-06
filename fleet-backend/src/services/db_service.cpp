#include "services/db_service.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <iostream>
#include <memory>


DBService::DBService(const nlohmann::json &config) : config_(config) {}

void DBService::init() {
  std::string db_path = "fleet.db";
  if (config_.contains("database") && config_["database"].contains("path")) {
      db_path = config_["database"]["path"].get<std::string>();
  }

  db_ = std::make_unique<SQLite::Database>(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  db_->exec("PRAGMA journal_mode=WAL;");
  db_->exec("PRAGMA synchronous=NORMAL");

  db_->exec(R"(
      CREATE TABLE IF NOT EXISTS nodes(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        hostname TEXT NOT NULL,
        ip TEXT NOT NULL,
        os TEXT,
        agent_version TEXT,
        last_heartbeat TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        status TEXT DEFAULT 'online',
        specs JSON
      )
  )");

  db_->exec(R"(
          CREATE TABLE IF NOT EXISTS audit_logs (
              id INTEGER PRIMARY KEY AUTOINCREMENT,
              node_id TEXT,
              user_id TEXT,
              command TEXT,
              exit_code INTEGER,
              executed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
              stdout TEXT,
              stderr TEXT
          );
      )");

    std::cout << "[DB] SQLite initialized: " << db_path << " (WAL mode enabled)\n";
}


std::string DBService::register_node(const nlohmann::json &info) {
    SQLite::Statement query(*db_,
        "INSERT INTO nodes (hostname, ip, os, agent_version, specs) VALUES (?, ?, ?, ?, ?)");

    query.bind(1, info["hostname"].get<std::string>());
    query.bind(2, info["ip"].get<std::string>());
    query.bind(3, info.value("os", "linux"));
    query.bind(4, info.value("agent_version", "1.0.0"));
    query.bind(5, info.dump());

    query.exec();

    auto node_id = db_->getLastInsertRowid();
    return std::to_string(node_id);
}


void DBService::update_heartbeat(const std::string &node_id) {
    SQLite::Statement query(*db_, "UPDATE nodes SET last_heartbeat = CURRENT_TIMESTAMP, status = 'online' WHERE id = ?");
    query.bind(1, node_id);
    query.exec();
}

std::vector<nlohmann::json> DBService::get_active_nodes() {
    std::vector<nlohmann::json> nodes;
    SQLite::Statement query(*db_, "SELECT id, hostname, ip, os, agent_version, specs FROM nodes WHERE last_heartbeat > datetime('now', '-5 minutes')");

    while (query.executeStep()) {
        nlohmann::json node;
        node["id"] = query.getColumn(0).getText();
        node["hostname"] = query.getColumn(1).getText();
        node["ip"] = query.getColumn(2).getText();
        node["os"] = query.getColumn(3).getText();
        node["agent_version"] = query.getColumn(4).getText();
        node["specs"] = nlohmann::json::parse(query.getColumn(5).getText());
        nodes.push_back(node);
    }
    return nodes;
}

nlohmann::json DBService::get_node_by_id(const std::string& node_id) {
    SQLite::Statement query(*db_, "SELECT ip FROM nodes WHERE id = ?;");
    query.bind(1, node_id);
    if (query.executeStep()) {
        nlohmann::json node;
        node["ip"] = query.getColumn(0).getString();
        return node;
    }
    return {};
}

std::string DBService::get_id_by_hostname(const std::string &hostname) {
    SQLite::Statement query(*db_, "SELECT id FROM nodes WHERE hostname = ?");
    query.bind(1, hostname);

    if (query.executeStep()) {
        int id = query.getColumn(0).getInt();
        return std::to_string(id);
    }

    return "";
}


void DBService::log_command(const std::string &node_id, const std::string &command,
                           int exit_code, const std::string &stdout_str,
                           const std::string &stderr_str) {
    SQLite::Statement query(*db_, "INSERT INTO audit_logs (node_id, command, exit_code, stdout, stderr) VALUES (?, ?, ?, ?, ?)");
    query.bind(1, node_id);
    query.bind(2, command);
    query.bind(3, exit_code);
    query.bind(4, stdout_str);
    query.bind(5, stderr_str);
    query.exec();
}
