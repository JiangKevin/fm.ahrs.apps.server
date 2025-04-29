#include "websocket_server.hpp"
#include <chrono>
#include <iostream>

WebSocketServer::WebSocketServer() : ioc_( 1 ), running_( false ), acceptor_( ioc_, tcp::endpoint( tcp::v4(), 0 ) ) {}

WebSocketServer::~WebSocketServer()
{
    stop();
}

void WebSocketServer::setPort( const std::string& port, moodycamel::ConcurrentQueue< SENSOR_DB >* sensor_data_queue )
{
    port_ = port;
    acceptor_.close();
    acceptor_.open( tcp::v4() );
    acceptor_.set_option( net::ip::tcp::acceptor::reuse_address( true ) );
    acceptor_.bind( tcp::endpoint( tcp::v4(), static_cast< unsigned short >( std::stoi( port_ ) ) ) );
    acceptor_.listen();
    //
    sensor_data_queue_ = sensor_data_queue;
}

void WebSocketServer::start()
{
    if ( ! running_ )
    {
        running_      = true;
        serverThread_ = std::thread(
            [ this ]()
            {
                acceptConnections();
                // std::thread sendThread( &WebSocketServer::handleSend, this );
                // sendThread.join();
            } );
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
        {
            std::lock_guard< std::mutex > lock( connectionsMutex_ );
            for ( auto conn : connections_ )
            {
                try
                {
                    conn->close( websocket::close_code::normal );
                    delete conn;
                }
                catch ( ... )
                {
                }
            }
            connections_.clear();
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
            auto ws = new websocket::stream< tcp::socket >( std::move( socket ) );
            {
                std::lock_guard< std::mutex > lock( connectionsMutex_ );
                connections_.push_back( ws );
            }
            //
            std::thread(
                [ this, ws ]()
                {
                    ws->accept();
                    handleReceive( *ws );
                } )
                .detach();
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Server main exception: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleReceive( websocket::stream< tcp::socket >& ws )
{
    try
    {
        beast::flat_buffer buffer;
        while ( running_ )
        {
            ws.read( buffer );
            std::string message = beast::buffers_to_string( buffer.data() );
            //
            SENSOR_DB sensor_data;
            sensor_data.getValueFromString( message );
            sensor_data_queue_->enqueue( sensor_data );
            //
            // std::cout << "Server received: " << message << std::endl;
            buffer.consume( buffer.size() );
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Server receive exception: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleSend( std::string message )
{
    std::lock_guard< std::mutex > lock( connectionsMutex_ );
    for ( auto conn : connections_ )
    {
        try
        {
            conn->write( net::buffer( message ) );
        }
        catch ( ... )
        {
            //
        }
    }
}