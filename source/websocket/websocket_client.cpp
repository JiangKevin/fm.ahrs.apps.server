#include "websocket_client.hpp"
#include <iostream>

WebSocketClient::WebSocketClient() : running_( false ) {}

WebSocketClient::~WebSocketClient()
{
    stop();
}

void WebSocketClient::setHost( const std::string& host )
{
    host_ = host;
}

void WebSocketClient::setPort( const std::string& port )
{
    port_ = port;
}

void WebSocketClient::setText( const std::string& text )
{
    text_ = text;
}

void WebSocketClient::start()
{
    if ( ! running_ )
    {
        running_      = true;
        clientThread_ = std::thread( &WebSocketClient::run, this );
    }
}

void WebSocketClient::stop()
{
    if ( running_ )
    {
        running_ = false;
        if ( clientThread_.joinable() )
        {
            clientThread_.join();
        }
    }
}

void WebSocketClient::run()
{
    try
    {
        net::io_context                  ioc;
        tcp::resolver                    resolver( ioc );
        websocket::stream< tcp::socket > ws( ioc );

        auto const results = resolver.resolve( host_, port_ );
        net::connect( ws.next_layer(), results.begin(), results.end() );
        ws.handshake( host_, "/" );
        ws.write( net::buffer( text_ ) );

        beast::flat_buffer buffer;
        ws.read( buffer );

        std::cout << "Client received: " << beast::buffers_to_string( buffer.data() ) << std::endl;
        ws.close( websocket::close_code::normal );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Client exception: " << e.what() << std::endl;
    }
}