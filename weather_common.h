
#ifndef WEATHER_COMMON_H
#define WEATHER_COMMON_H

#include "base64string.h"
#include "weather_data.pb.h"

// Trace printing for WeatherData protobuf.
std::ostream&
operator<<( std::ostream& os,
            const zweather::WeatherData& data_point );

// Trace printing for TapStats protobuf.
std::ostream&
operator<<( std::ostream& os,
            const zweather::TapStats& stats );

// Convert WeatherData protobuf to base 64 encoded string.
Base64String
ProtoBufToBase64( const zweather::WeatherData& data_point );
#endif // WEATHER_COMMON_H
