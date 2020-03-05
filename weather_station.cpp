
#include <iostream>
#include <string>

#include <sys/time.h>
#include <unistd.h>

#include <zmq.hpp>

#include "weather_common.h"
#include "weather_data.pb.h"
#include "tune.h"


static Tune
setup_tuning_variables() {
    Tune tune( "WEATHER_STATION_" );

    tune.add_variable( "SERVER_HOSTNAME", "localhost" );
    tune.add_variable( "SERVER_PORT", "5555" );
    tune.add_variable( "SAMPLE_INTERVAL", "3" );

    return tune;
}


int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Tune tune = setup_tuning_variables();

    const unsigned long data_interval =
        std::stoul( tune.get( "SAMPLE_INTERVAL" ) );

    zmq::context_t context( 1 );
    zmq::socket_t socket( context, ZMQ_PUSH );

    socket.connect ( "tcp://" + tune.get( "SERVER_HOSTNAME" ) + ":" +
                     tune.get( "SERVER_PORT" ) );

    zweather::WeatherData data_point;

    data_point.set_authentication_token("secret");

    // The station ID will probably be a UUID in a real endpoint.  Use PID for
    // testing.
    std::ostringstream station_id;
    station_id << "station" << getpid();
    data_point.set_station_id(station_id.str());
    data_point.mutable_location()->set_latitude(37.600833);
    data_point.mutable_location()->set_longitude(-122.401389);

    while(true) {
        // FIXME  We are sending dummy data and it is all the same.
        data_point.set_temperature(12.22);  // degrees celsius
        data_point.set_humidity(57);  // percent

        struct timeval tv;
        gettimeofday(&tv, NULL);

        data_point.mutable_sample_time()->set_seconds(tv.tv_sec);
        data_point.mutable_sample_time()->set_nanos(tv.tv_usec * 1000);

        data_point.mutable_send_time()->set_seconds(tv.tv_sec);
        data_point.mutable_send_time()->set_nanos(tv.tv_usec * 1000);

        std::cout << "Sending sample:\n";
        std::cout << data_point << "\n\n";

        std::string buf;
        bool good = data_point.SerializeToString(&buf);
        if (! good) {
            std::cerr << "Error serializing" << std::endl;
            return 1;
        }
        zmq::message_t request(buf.size());
        memcpy(request.data(), buf.c_str(), buf.size());
        int rc = socket.send(request);
        if (rc == -1) {
            throw( std::runtime_error( "Error sending" ) );
        }

        sleep( data_interval );
    }
}
