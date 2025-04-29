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
            } );
    }
}

void WebSocketServer::stop()
{
    printf( "WebSocketServer::stop()\n" );
    //
    if ( running_ )
    {
        running_ = false;
        ioc_.stop();
        //
        if ( serverThread_.joinable() )
        {
            printf( "Server thread joined : %d\n", serverThread_.get_id() );
            // serverThread_.join();
        }
        {
            std::lock_guard< std::mutex > lock( connectionsMutex_ );
            //
            for ( auto net_ptr : net_ptrs_ )
            {
                try
                {
                    net_ptr.connection_->close( websocket::close_code::normal );
                    //
                    if ( net_ptr.thread_->joinable() )
                    {
                        printf( "net_ptr.thread_ joined : %d\n", net_ptr.thread_->get_id() );
                        // net_ptr.thread_->join();
                        delete net_ptr.thread_;
                    }
                    //
                    delete net_ptr.connection_;
                }
                catch ( ... )
                {
                    //
                }
            }
            net_ptrs_.clear();
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
            //
            auto ws = new websocket::stream< tcp::socket >( std::move( socket ) );
            {
                std::lock_guard< std::mutex > lock( connectionsMutex_ );
            }
            //
            std::thread rec_thread = std::thread(
                [ this, ws ]()
                {
                    ws->accept();
                    handleReceive( *ws );
                } );
            //
            rec_thread.detach();

            //
            NET_PTR net_ptr;
            net_ptr.connection_ = ws;
            net_ptr.thread_     = &rec_thread;
            net_ptrs_.push_back( net_ptr );
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
    //
    for ( auto net_ptr : net_ptrs_ )
    {
        try
        {
            net_ptr.connection_->write( net::buffer( message ) );
        }
        catch ( ... )
        {
            //
        }
    }
}