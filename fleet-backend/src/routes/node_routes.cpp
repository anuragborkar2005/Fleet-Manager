#include "services/db_service.hpp"
#include "services/agent_client.hpp"

#include <crow.h>
#include <crow/app.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <crow/middlewares/cors.h>
#include <nlohmann/json.hpp>



void register_nodes_routes(crow::App<crow::CORSHandler>& app, AgentClient& agent_client, DBService& db) {
    CROW_ROUTE(app, "/api/nodes/register")
        .methods("POST"_method)([&db](const crow::request &req) {
            try {
                auto body = nlohmann::json::parse(req.body);
                std::string node_id = db.register_node(body);

                crow::json::wvalue res;
                res["node_id"] = node_id;
                res["status"] = "registered";
                res["jwt"] = "node-jwt-placeholder-" + body["hostname"].get<std::string>();

                return crow::response{200, res};
            } catch (const std::exception& e) {
                return crow::response{400, "Invalid JSON"};
            }
        });

    CROW_ROUTE(app, "/api/nodes")
    ([&db](const crow::request &) {
        auto nodes = db.get_active_nodes();

        std::vector<crow::json::wvalue> res_list;
        for (const auto& node : nodes) {
            crow::json::wvalue node_json;
            node_json["id"] = node.value("id", "");
            node_json["hostname"] = node.value("hostname", "unknown");
            node_json["ip"] = node.value("ip", "0.0.0.0");
            node_json["os"] = node.value("os", "linux");
            node_json["agent_version"] = node.value("agent_version", "1.0.0");
            node_json["status"] = node.value("status", "offline");
            res_list.push_back(std::move(node_json));
        }

        return crow::response{200, crow::json::wvalue(std::move(res_list))};
    });

    CROW_ROUTE(app, "/api/nodes/heartbeat")
    .methods("POST"_method)([&db](const crow::request &req) {
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("node_id")) {
                return crow::response{400, "Missing node_id"};
            }
            db.update_heartbeat(body["node_id"].get<std::string>());
            return crow::response{200, "OK"};
        } catch (const std::exception& e) {
            return crow::response{400, "Invalid JSON"};
        }
    });

    CROW_ROUTE(app, "/prometheus/sd")
    ([&db](const crow::request &) {
      auto nodes = db.get_active_nodes();

      crow::json::wvalue targets = crow::json::wvalue(crow::json::type::List);

      for (auto &node : nodes) {
        crow::json::wvalue item;
        item["targets"] = crow::json::wvalue(crow::json::type::List);
        item["targets"][0] = node["ip"].get<std::string>() + ":8082"; // Using metrics port

        item["labels"] = crow::json::wvalue::object();
        item["labels"]["job"] = "fleet-agent";
        item["labels"]["node_id"] = node["id"].get<std::string>();
        item["labels"]["hostname"] = node["hostname"].get<std::string>();

        targets[targets.size()] = std::move(item);
      }

      return crow::response{targets};
    });

    CROW_ROUTE(app, "/api/nodes/<string>/execute").methods("POST"_method)
        ([&db , &agent_client ](const crow::request& req, std::string node_id) {
            auto body = crow::json::load(req.body);
            if (!body || !body.has("command"))
                return crow::response(400, "Missing command");

            std::string command = body["command"].s();

            auto node = db.get_node_by_id(node_id);
            if (node.empty())
                return crow::response(404, "Node not found");

            auto agent_response = agent_client.execute_command(node["ip"], command);

            db.log_command(node_id, command, agent_response.value("exit_code", 0), 
                          agent_response.value("stdout", ""), 
                          agent_response.value("stderr", ""));

            crow::json::wvalue resp;
            resp["status"] = "success";
            resp["node_id"] = node_id;
            resp["stdout"] = agent_response.value("stdout", "");
            resp["stderr"] = agent_response.value("stderr", "");
            resp["exit_code"] = agent_response.value("exit_code", 0);
            return crow::response{resp};
    });

    CROW_ROUTE(app, "/metrics")
        ([]() {
            crow::response res(200);
            res.set_header("Content-Type", "text/plain");
            res.write("# HELP fleet_backend_up Backend is running\n");
            res.write("fleet_backend_up 1\n");
            return res;
    });

}
