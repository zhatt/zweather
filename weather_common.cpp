
#include <iostream>
#include <string>

#include "weather_common.h"

std::ostream&
operator<<( std::ostream& os,
            const zweather::WeatherData& data_point ) {

    // Protobufs doesn't know the length on the wire without serializing so we
    // serialize the value.  Report -1 in the unlikely event of an error.
    int num_bytes = -1;
    std::string buf;
    bool good = data_point.SerializeToString(&buf);
    if (good ) {
        num_bytes = buf.size();
    }

    os <<   "token:        " << data_point.authentication_token()
       << "\nstation:      " << data_point.station_id()

       << "\nlatitude:     " << data_point.location().latitude()
       << "\nlongitude:    " << data_point.location().longitude()

       << "\nsample_time:  " << data_point.sample_time().seconds()
       <<               ", " << data_point.sample_time().nanos()

       << "\nsend_time:    " << data_point.send_time().seconds()
       <<               ", " << data_point.send_time().nanos()

       << "\nreceive_time: " << data_point.receive_time().seconds()
       <<               ", " << data_point.receive_time().nanos()

       << "\ntemperature:  " << data_point.temperature()
       << "\nhumidity:     " << data_point.humidity()

       << "\nbytes:        " << num_bytes;
       // No final newline.  Sender should add that if needed.

    return os;
}
