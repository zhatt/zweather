
#ifndef COMMON_H
#define COMMON_H

#include "base64string.h"
#include "weather_data.pb.h"

std::ostream&
operator<<( std::ostream& os,
            const zweather::WeatherData& data_point );

Base64String
ProtoBufToBase64( const zweather::WeatherData& data_point );
#endif
