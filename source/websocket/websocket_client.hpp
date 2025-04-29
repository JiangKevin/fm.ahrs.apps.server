#ifndef WEBSOCKET_CLIENT_HPP
#define WEBSOCKET_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <string>
#include <thread>

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = net::ip::tcp;

class WebSocketClient
{
public:
    WebSocketClient();
    ~WebSocketClient();
    void setHost( const std::string& host );
    void setPort( const std::string& port );
    void setText( const std::string& text );
    void start();
    void stop();
private:
    void run();

    std::string host_;
    std::string port_;
    std::string text_;
    std::thread clientThread_;
    bool        running_;
};

#endif