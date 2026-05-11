#include "agent/agent.hpp"
#include "routes/routes.hpp"
#include <crow.h>
#include <iostream>

int main()
{
    crow::SimpleApp app;
    Agent agent;

    int port = agent.get_port();
    std::cout << "[Main] Starting agent on port " << port << std::endl;

    agent.start_registration_and_heartbeat();

    register_routes(app, agent);
    app.port(port).multithreaded().run();
}
