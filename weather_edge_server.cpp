#include <string>
#include <iostream>

#include <sys/time.h>

#include <zmq.hpp>

#include "statsmonitortap.h"
#include "tune.h"
#include "weather_common.h"
#include "weather_data.pb.h"


static Tune
setup_tuning_variables() {
    Tune tune( "WEATHER_EDGE_SERVER_" );

    tune.add_variable( "LISTEN_PORT", "5555" );
    tune.add_variable( "TAP_PORT", "6555" );

    tune.add_variable( "SQL_STORE_HOSTNAME", "localhost" );
    tune.add_variable( "SQL_STORE_PORT", "5556" );

    tune.add_variable( "RAW_STORE_HOSTNAME", "localhost" );
    tune.add_variable( "RAW_STORE_PORT", "5557" );

    return tune;
}


int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Tune tune = setup_tuning_variables();

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a socket and bind to interface for receiving data.
    zmq::socket_t in_socket{ context, ZMQ_PULL };

    // The weather station sender is untrusted.  Limit the max message size.
    // This needs set before binding.
    int64_t max_message_size = 1000;
    in_socket.setsockopt( ZMQ_MAXMSGSIZE, &max_message_size,
                          sizeof( max_message_size ) );

    in_socket.bind( "tcp://*:" + tune.get( "LISTEN_PORT" ) );

    // Construct a socket and bind to interface for sending to SQL store.
    zmq::socket_t out_sql_store_socket{ context, ZMQ_PUSH };
    out_sql_store_socket.connect( "tcp://" + tune.get( "SQL_STORE_HOSTNAME" ) +
                                  ":" + tune.get( "SQL_STORE_PORT" ) );

    // Construct a socket and bind to interface for sending to raw store.
    zmq::socket_t out_raw_store_socket{ context, ZMQ_PUSH };
    out_raw_store_socket.connect( "tcp://" + tune.get( "RAW_STORE_HOSTNAME" ) +
                                  ":" + tune.get( "RAW_STORE_PORT" ) );

    StatsMonitorTap stats_tap( context,
                               std::string( tune.get( "TAP_PORT" ) ) );

    for (;;) {
        zmq::message_t message_in;

        // receive a request from client
        in_socket.recv( message_in, zmq::recv_flags::none );
        stats_tap.add_bytes_received( message_in.size() );

        struct timeval receive_time;
        gettimeofday( &receive_time, NULL );

        std::string data( reinterpret_cast<const char*>( message_in.data() ) );
        zweather::WeatherData data_point;
        data_point.ParseFromString( data );

        // FIXME validate buffer.  It comes from untrusted client.
        // We need to validate the secret, etc.
        data_point.mutable_receive_time()->set_seconds( receive_time.tv_sec );
        data_point.mutable_receive_time()->set_nanos( receive_time.tv_usec * 1000 );

        // FIXME remove or make trace conditional.
        std::cout << "Received " << std::endl;
        std::cout << data_point << "\n\n";

        // Reserialize with receive time.
        std::string buf;
        bool good = data_point.SerializeToString( &buf );
        if ( ! good ) {
            std::cerr << "Error serializing" << std::endl;
            return 1;
        }
        zmq::message_t message_to_sql_store( buf.size() );
        memcpy( message_to_sql_store.data(), buf.c_str(), buf.size() );

        // Send the message to both the sql and raw store backends.
        zmq::message_t message_to_raw_store;
        message_to_raw_store.copy( message_to_sql_store );

        // Sending releases the message so grab the size.
        unsigned size = message_to_sql_store.size();
        out_sql_store_socket.send( message_to_sql_store );
        stats_tap.add_bytes_sent( size );

        size = message_to_raw_store.size();
        out_raw_store_socket.send( message_to_raw_store );
        stats_tap.add_bytes_sent( size );

        stats_tap.service_tap();
    }

    return 0;
}
