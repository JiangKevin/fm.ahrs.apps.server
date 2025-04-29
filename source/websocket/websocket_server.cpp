#include "websocket_server.hpp"
#include <iostream>
#include <chrono>

WebSocketServer::WebSocketServer() : ioc_(1), running_(false), acceptor_(ioc_, tcp::endpoint(tcp::v4(), 0)) {}

WebSocketServer::~WebSocketServer() {
    stop();
}

void WebSocketServer::setPort(const std::string& port) {
    port_ = port;
    acceptor_.close();
    acceptor_.open(tcp::v4());
    acceptor_.set_option(net::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(tcp::endpoint(tcp::v4(), static_cast<unsigned short>(std::stoi(port_))));
    acceptor_.listen();
}

void WebSocketServer::start() {
    if (!running_) {
        running_ = true;
        serverThread_ = std::thread([this]() {
            acceptConnections();
            sendPeriodicMessages();
        });
    }
}

void WebSocketServer::stop() {
    if (running_) {
        running_ = false;
        ioc_.stop();
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
        {
            std::lock_guard<std::mutex> lock(connectionsMutex_);
            for (auto conn : connections_) {
                try {
                    conn->close(websocket::close_code::normal);
                    delete conn;
                } catch (...) {}
            }
            connections_.clear();
        }
    }
}

void WebSocketServer::acceptConnections() {
    try {
        while (running_) {
            tcp::socket socket{ioc_};
            acceptor_.accept(socket);
            auto ws = new websocket::stream<tcp::socket>(std::move(socket));
            {
                std::lock_guard<std::mutex> lock(connectionsMutex_);
                connections_.push_back(ws);
            }
            std::thread([this, ws]() {
                handleConnection(std::move(*ws));
            }).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Server main exception: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleConnection(websocket::stream<tcp::socket> ws) {
    try {
        ws.accept();
        beast::flat_buffer buffer;
        while (running_) {
            ws.read(buffer);
            std::string message = beast::buffers_to_string(buffer.data());
            std::cout << "Server received: " << message << std::endl;
            buffer.consume(buffer.size());
        }
    } catch (const std::exception& e) {
        std::cerr << "Server connection exception: " << e.what() << std::endl;
    }
}

void WebSocketServer::sendPeriodicMessages() {
    while (running_) {
        {
            std::lock_guard<std::mutex> lock(connectionsMutex_);
            for (auto conn : connections_) {
                try {
                    conn->write(net::buffer("Periodic message from server"));
                } catch (...) {}
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}