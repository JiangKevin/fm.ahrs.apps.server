#ifndef WEBSOCKET_SERVER_HPP
#define WEBSOCKET_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <thread>
#include <string>
#include <vector>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();
    void setPort(const std::string& port);
    void start();
    void stop();

private:
    void acceptConnections();
    void handleConnection(websocket::stream<tcp::socket> ws);
    void sendPeriodicMessages();

    std::string port_;
    net::io_context ioc_;
    tcp::acceptor acceptor_;
    std::thread serverThread_;
    bool running_;
    std::vector<websocket::stream<tcp::socket>*> connections_;
    std::mutex connectionsMutex_;
};

#endif