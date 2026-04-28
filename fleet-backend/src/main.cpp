#include <crow.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Starting Fleet Backend...");
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/health")([](){
        return "Fleet Backend is healthy";
    });

    app.port(8080).multithreaded().run();
}
