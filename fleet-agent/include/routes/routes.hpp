#pragma once
#include "agent/agent.hpp"
#include <crow.h>
void register_routes(crow::SimpleApp &app, Agent &agent);
