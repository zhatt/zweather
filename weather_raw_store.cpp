#include <string>
#include <iostream>

#include <zmq.hpp>

#include "weather_common.h"
#include "weather_data.pb.h"

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a response socket and bind to interface
    zmq::socket_t socket{context, ZMQ_PULL};
    // FIXME move to common or make configurable.
    socket.bind("tcp://*:5557");

    for (;;) {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);

        std::cout << "RAW server received " << std::endl;
        zweather::WeatherData data_point;

        std::string data(reinterpret_cast<const char*>(request.data()));
        data_point.ParseFromString(data);

        std::cout << data_point << "\n\n";

        // FIXME remove if we still use PULL
        //zmq::message_t reply;
        //socket.send(reply);
    }

    return 0;
}
