#include <string>
#include <iostream>

#include <zmq.hpp>

#include "tune.h"
#include "statsmonitortap.h"
#include "weather_common.h"
#include "weather_data.pb.h"


static Tune
setup_tuning_variables() {
    Tune tune( "WEATHER_SQL_STORE_" );

    tune.add_variable( "LISTEN_PORT", "5556" );
    tune.add_variable( "TAP_PORT", "6556" );

    tune.add_variable( "THROTTLE_TIME", "60" );

    return tune;
}

class Throttler {
    const unsigned long throttle_time; // Seconds

    //  Last time a station was allowed to insert.
    std::map<std::string,uint64_t> last_inserted_times;

  public:
    Throttler( uint throttle_time ) :
        throttle_time( throttle_time ) {}

    // Check if we can do an insert for the station.  Return true if we can.
    // We also update the last inserted time for the station.  I.E.  You are
    // expected to perform the insert operation if you can.
    bool can_insert( const std::string station_id ) {

        struct timespec now_timespec;
        int ret = clock_gettime( CLOCK_MONOTONIC, &now_timespec );
        assert( ret == 0 );
        uint64_t now = now_timespec.tv_sec;

        auto iter = this->last_inserted_times.find( station_id );

        // First time we've seen this station.  Allow insert.
        if ( iter == this->last_inserted_times.end() ) {
            last_inserted_times[station_id] = now;
            return true;
        }

        // Otherwise check if it time to insert.
        uint64_t last_inserted_time = iter->second;
        if ( now - last_inserted_time >= throttle_time ) {
            last_inserted_times[station_id] = now;
            return true;
        }

        return false;
    }

};


int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Tune tune = setup_tuning_variables();

    Throttler throttler( std::stoul( tune.get( "THROTTLE_TIME" ) ) );

    // Initialize the zmq context with a single IO thread.
    zmq::context_t context{1};

    // Construct a response socket and bind to interface
    zmq::socket_t socket{ context, ZMQ_PULL };
    socket.bind( "tcp://*:" + tune.get( "LISTEN_PORT" ) );


    StatsMonitorTap stats_tap( context,
                               std::string( tune.get( "TAP_PORT" ) ) );

    for (;;) {
        zmq::message_t request;

        // Receive a request from edge server.
        socket.recv( request, zmq::recv_flags::none );
        stats_tap.add_bytes_received( request.size() );

        zweather::WeatherData data_point;
        std::string data( reinterpret_cast<const char*>( request.data() ) );
        data_point.ParseFromString( data );

        if ( throttler.can_insert( data_point.station_id() ) ) {
            std::cout << "Inserting in SQL\n";
            // TODO add SQL backend.
        } else {
            std::cout << "Not inserting in SQL\n";
        }
        std::cout << data_point << "\n\n";

        stats_tap.service_tap();
    }

    return 0;
}
