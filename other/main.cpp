#include "websocket_server.hpp"
#include "websocket_client.hpp"
#include <chrono>

int main() {
    std::string host = "127.0.0.1";
    std::string port = "8080";
    std::string text = "Hello, WebSocket Server!";

    WebSocketServer server;
    server.setPort(port);

    WebSocketClient client;
    client.setHost(host);
    client.setPort(port);
    client.setText(text);

    server.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.start();

    client.stop();
    server.stop();

    return 0;
}    