#include "utils/jwt_validator.hpp"
#include "routes/routes.hpp"
#include "agent/secure_exec.hpp"

#include <crow/json.h>
#include <nlohmann/json.hpp>
#include <crow/app.h>
#include <crow/http_request.h>
#include <crow/http_response.h>

using json = nlohmann::json;

void register_routes(crow::SimpleApp &app, Agent &agent)
{

    CROW_ROUTE(app, "/api/register")
    ([&]()
     {
      agent.start_registration_and_heartbeat();
      return crow::response{200, "Node Registered"}; });

    CROW_ROUTE(app, "/api/set_heartbeat")
        .methods("POST"_method)([&](const crow::request &req)
                                {
          agent.metrics->incrementHeartbeat();

          auto body = json::parse(req.body);
          std::string hostname = body.value("hostname", "unknown");

          return crow::response{200, "Heartbeat recorded for " + hostname}; });

    CROW_ROUTE(app, "/info").methods("POST"_method)([&](const crow::request &req)
                                                    {
      auto body = json::parse(req.body);
      std::string hostname = body.value("hostname", "unknown");
      return crow::response{200, "Received Hostname : " + hostname}; });

    CROW_ROUTE(app, "/api/metrics")
    ([&]()
     { return crow::response{200, "Metrics not implemented"}; });

    CROW_ROUTE(app, "/api/status")
    ([&]()
     {
      crow::json::wvalue x;
      x["status"] = "healthy";
      x["uptime_seconds"] =
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::steady_clock::now().time_since_epoch())
              .count();
      return x; });

    CROW_ROUTE(app, "/api/execute")
        .methods("POST"_method)([&agent](const crow::request &req)
                                {
          std::string auth = req.get_header_value("Authorization");
            if (auth.empty() || !auth.starts_with("Bearer ")) {
                return crow::response{401, "Unauthorized"};
            }
            std::string token = auth.substr(7);

            if (!validate_jwt(token, agent.get_jwt())) {
                return crow::response{401, "Invalid token"};
            }

            auto body = crow::json::load(req.body);
            if (!body) {
              return crow::response{400, "Bad JSON format"};
            }

            std::string command = body["command"].s();
            auto [exit_code, stdout_str, stderr_str] = secure_execute(command, 60);


            crow::json::wvalue result;
            result["exit_code"] = exit_code;
            result["stdout"] = stdout_str;
            result["stderr"] = stderr_str;
            return crow::response{result}; });
}
