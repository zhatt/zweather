
#ifndef COMMON_H
#define COMMON_H

#include "weather_data.pb.h"

std::ostream&
operator<<( std::ostream& os,
            const zweather::WeatherData& data_point );

#endif
