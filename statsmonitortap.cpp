#include <iostream>

#include "statsmonitortap.h"
#include "weather_common.h"
#include "weather_data.pb.h"


StatsMonitorTap::StatsMonitorTap( zmq::context_t& zmq_context,
                     const std::string& listener_port ) :
    tap_socket( zmq_context, ZMQ_REP )
{
    tap_socket.bind( "tcp://*:" + listener_port );
}

void
StatsMonitorTap::add_bytes_received( unsigned long bytes ) {
    bytes_received += bytes;
}

void
StatsMonitorTap::add_bytes_sent( unsigned long bytes ) {
    bytes_sent += bytes;
}

void
StatsMonitorTap::service_tap() {
    zmq::message_t request;

    bool rc;
    do {
       zmq::message_t message;
       if ( ( rc = tap_socket.recv( &message, ZMQ_DONTWAIT ) ) == true ) {

            // Serialize and send current stats back to the client.
            zweather::TapStats stats;
            stats.set_bytes_received( bytes_received );
            stats.set_bytes_sent( bytes_sent );

            std::string buf;
            bool good = stats.SerializeToString( &buf );
            if ( ! good ) {
                std::cerr << "Error serializing" << std::endl;
                exit( 1 );
            }

            zmq::message_t reply( buf.size() );
            memcpy( reply.data(), buf.c_str(), buf.size() );

            tap_socket.send( reply, ZMQ_DONTWAIT );
       }
    } while( rc == true );
}
