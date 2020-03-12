#include <string>
#include <iostream>

#include <unistd.h>
#include <zmq.hpp>

#include "weather_common.h"
#include "weather_data.pb.h"
#include "tune.h"


static Tune
setup_tuning_variables() {
    Tune tune( "TAP_MONITOR_" );

    tune.add_variable( "EDGE_SERVER_HOSTNAME", "localhost" );
    tune.add_variable( "EDGE_SERVER_TAP_PORT", "6555" );

    tune.add_variable( "MONITOR_INTERVAL", "60" );

    return tune;
}


int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Starting tap monitor" << std::endl;

    Tune tune = setup_tuning_variables();

     const unsigned long monitor_interval =
        std::stoul( tune.get( "MONITOR_INTERVAL" ) );


    // Initialize the zmq context with a single IO thread.
    zmq::context_t context{1};

    // Construct a response socket and bind to interface
    zmq::socket_t socket{ context, ZMQ_REQ };

    socket.connect ( "tcp://" + tune.get( "EDGE_SERVER_HOSTNAME" ) + ":" +
                     tune.get( "EDGE_SERVER_TAP_PORT" ) );

    for (;;) {
        // FIXME make non-blocking and monitor the other services.

        // Send a request for data.  This is just an empty request.  The tap
        // will always send the stats back.  This should be made non-blocking
        // or have a timeout.  We could then detect a failed endpoint and take
        // a recovery action.
        zmq::message_t request(0);
        socket.send( request );

        // Get the reply protobuf.
        zmq::message_t reply;
        socket.recv( reply, zmq::recv_flags::none );

        // Convert zmq message to protobuf and print it.
        zweather::TapStats stats;
        std::string data( reinterpret_cast<const char*>( reply.data() ) );
        stats.ParseFromString( data );

        std::cout << stats << "\n" << std::endl;

        sleep( monitor_interval );
    }

    return 0;
}
