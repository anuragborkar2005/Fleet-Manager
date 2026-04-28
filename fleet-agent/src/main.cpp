#include <crow.h>
#include <fmt/core.h>

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/health")([](){
        return "Fleet Agent is healthy";
    });

    fmt::print("Starting Fleet Agent on port 8081...\n");
    app.port(8081).multithreaded().run();
}
