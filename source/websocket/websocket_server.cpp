#include "websocket_server.hpp"
#include <iostream>

WebSocketServer::WebSocketServer() : ioc_( 1 ), running_( false ), acceptor_( ioc_, tcp::endpoint( tcp::v4(), 0 ) ) {}

WebSocketServer::~WebSocketServer()
{
    stop();
}

void WebSocketServer::setPort( const std::string& port )
{
    port_ = port;
    acceptor_.close();
    acceptor_.open( tcp::v4() );
    acceptor_.set_option( net::ip::tcp::acceptor::reuse_address( true ) );
    acceptor_.bind( tcp::endpoint( tcp::v4(), static_cast< unsigned short >( std::stoi( port_ ) ) ) );
    acceptor_.listen();
}

void WebSocketServer::start()
{
    if ( ! running_ )
    {
        running_      = true;
        serverThread_ = std::thread( &WebSocketServer::acceptConnections, this );
    }
}

void WebSocketServer::stop()
{
    if ( running_ )
    {
        running_ = false;
        ioc_.stop();
        if ( serverThread_.joinable() )
        {
            serverThread_.join();
        }
    }
}

void WebSocketServer::acceptConnections()
{
    try
    {
        while ( running_ )
        {
            tcp::socket socket{ ioc_ };
            acceptor_.accept( socket );
            std::thread( &WebSocketServer::handleConnection, this, websocket::stream< tcp::socket >( std::move( socket ) ) ).detach();
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Server main exception: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleConnection( websocket::stream< tcp::socket > ws )
{
    try
    {
        ws.accept();
        beast::flat_buffer buffer;
        ws.read( buffer );
        std::string message = beast::buffers_to_string( buffer.data() );
        std::cout << "Server received: " << message << std::endl;
        ws.write( net::buffer( "Server received: " + message ) );
        ws.close( websocket::close_code::normal );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Server connection exception: " << e.what() << std::endl;
    }
}