#pragma once

#include "services/db_service.hpp"
#include "services/agent_client.hpp"

#include <crow/app.h>
#include <crow/middlewares/cors.h>

void register_nodes_routes(crow::App<crow::CORSHandler>& app, AgentClient& agent_client, DBService& db);
