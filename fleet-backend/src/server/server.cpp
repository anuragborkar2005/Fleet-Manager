#include "server/server.hpp"
#include "utils/config.hpp"
#include "routes/node_routes.hpp"

Server::Server() {
    config = load_config_file("config.json");
    db = std::make_unique<DBService>(config);
    agent_client = std::make_unique<AgentClient>(config);
    db->init();
}

void Server::register_routes() {
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("*")
        .methods("POST"_method, "GET"_method, "OPTIONS"_method)
        .headers("Content-Type", "Authorization");

    register_nodes_routes(app, *agent_client, *db);
}

void Server::run() {
    register_routes();
    int port = config["server"]["port"].get<int>();
    std::cout << "[Fleet Backend] Starting on http://0.0.0.0:" << port
              << " with SQLite database\n";
    app.port(port).multithreaded().run();
}
