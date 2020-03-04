#include <string>
#include <iostream>

#include <sys/time.h>

#include <zmq.hpp>

#include "weather_common.h"
#include "weather_data.pb.h"

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a socket and bind to interface for receiving data.
    zmq::socket_t in_socket{ context, ZMQ_PULL };
    in_socket.bind( "tcp://*:5555" );

    // construct a socket and bind to interface for sending to SQL store.
    zmq::socket_t out_sql_store_socket{ context, ZMQ_PUSH };
    out_sql_store_socket.connect( "tcp://localhost:5556" );

    // construct a socket and bind to interface for sending to raw store.
    zmq::socket_t out_raw_store_socket{ context, ZMQ_PUSH };
    out_raw_store_socket.connect( "tcp://localhost:5557" );

    for (;;) {
        zmq::message_t message_in;

        // receive a request from client
        in_socket.recv( message_in, zmq::recv_flags::none );

        struct timeval receive_time;
        gettimeofday( &receive_time, NULL );

        std::string data( reinterpret_cast<const char*>( message_in.data() ) );
        zweather::WeatherData data_point;
        data_point.ParseFromString( data );

        //FIXME validate buffer.  It comes from untrusted client.
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

        out_sql_store_socket.send( message_to_sql_store );
        out_raw_store_socket.send( message_to_raw_store );
    }

    return 0;
}
