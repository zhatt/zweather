
#include <iostream>
#include <string>

#include "base64string.h"
#include "rawbytes.h"
#include "weather_common.h"

std::ostream&
operator<<( std::ostream& os,
            const zweather::WeatherData& data_point ) {

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

       << "\nbytes:        " << data_point.ByteSizeLong();
       // No final newline.  Caller should add that if needed.

    return os;
}

std::ostream&
operator<<( std::ostream& os,
            const zweather::TapStats& stats ) {

    os <<   "rx: " << stats.bytes_received()
       << "\ntx: " << stats.bytes_sent();
       // No final newline.  Caller should add that if needed.

    return os;
}

// Convert the data_point protobuf to a base64 encoded string.  If the protobuf
// is corrupted, returns an empty string that is base64 encoded.
// FIXME figure out how to better handle corrupted protobuf.
Base64String
ProtoBufToBase64( const zweather::WeatherData& data_point ) {
    std::string tmp;

    bool good = data_point.SerializeToString(&tmp);
    // Return an empty object if we can't deserialize the protobuf.  This
    // means it is corrupted.
    if (!good) {
        tmp.clear();
    }

    return Base64String(RawBytes(tmp));
}

