#pragma once
//
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector< std::string > splitString( const std::string& str, char delimiter )
{
    std::vector< std::string > result;
    std::istringstream         iss( str );
    std::string                token;
    while ( std::getline( iss, token, delimiter ) )
    {
        result.push_back( token );
    }
    return result;
}
//
struct SENSOR_DB
{
    float time;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float quate_x;
    float quate_y;
    float quate_z;
    float quate_w;
    float roll;
    float pitch;
    float yaw;
    float pos_x;
    float pos_y;
    float pos_z;
    //
    std::string to_string()
    {
        return std::to_string( time ) + "," + std::to_string( acc_x ) + "," + std::to_string( acc_y ) + "," + std::to_string( acc_z ) + "," + std::to_string( gyro_x ) + "," + std::to_string( gyro_y ) + "," + std::to_string( gyro_z ) + "," + std::to_string( mag_x ) + "," + std::to_string( mag_y )
               + "," + std::to_string( mag_z ) + "," + std::to_string( quate_x ) + "," + std::to_string( quate_y ) + "," + std::to_string( quate_z ) + "," + std::to_string( quate_w ) + "," + std::to_string( roll ) + "," + std::to_string( pitch ) + "," + std::to_string( yaw ) + ","
               + std::to_string( pos_x ) + "," + std::to_string( pos_y ) + "," + std::to_string( pos_z );
    };
    //
    void getValueFromString( std::string v )
    {
        char delimiter = ',';
        auto values    = splitString( v, delimiter );
        // 
        if ( values.size() == 20 )
        {
            time    = std::stof( values[ 0 ] );
            acc_x   = std::stof( values[ 1 ] );
            acc_y   = std::stof( values[ 2 ] );
            acc_z   = std::stof( values[ 3 ] );
            gyro_x  = std::stof( values[ 4 ] );
            gyro_y  = std::stof( values[ 5 ] );
            gyro_z  = std::stof( values[ 6 ] );
            mag_x   = std::stof( values[ 7 ] );
            mag_y   = std::stof( values[ 8 ] );
            mag_z   = std::stof( values[ 9 ] );
            quate_x = std::stof( values[ 10 ] );
            quate_y = std::stof( values[ 11 ] );
            quate_z = std::stof( values[ 12 ] );
            quate_w = std::stof( values[ 13 ] );
            roll    = std::stof( values[ 14 ] );
            pitch   = std::stof( values[ 15 ] );
            yaw     = std::stof( values[ 16 ] );
            pos_x   = std::stof( values[ 17 ] );
            pos_y   = std::stof( values[ 18 ] );
            pos_z   = std::stof( values[ 19 ] );
        }
    }
};