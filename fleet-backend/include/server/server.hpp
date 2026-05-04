#pragma once
#include "services/db_service.hpp"
#include "services/agent_client.hpp"
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <nlohmann/json.hpp>

class Server {
public:
    Server();
    void run();

private:
    crow::App<crow::CORSHandler> app;
    std::unique_ptr<DBService> db;
    std::unique_ptr<AgentClient> agent_client;
    nlohmann::json config;

    void register_routes();
};
