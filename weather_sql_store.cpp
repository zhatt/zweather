#include <string>
#include <iostream>

#include <zmq.hpp>

#include "weather_common.h"
#include "weather_data.pb.h"

class Throttler {
    // FIXME add constructor to pass this in.
    const uint throttle_time = 60; // Seconds

    //  Last time a station was allowed to insert.
    std::map<std::string,uint64_t> last_inserted_times;

  public:
    // Check if we can do an insert for the station.  Return true if we can.
    // We also update the last inserted time for the station.  I.E.  You are
    // expected to perform the insert operation if you can.
    bool can_insert(const std::string station_id) {
        std::cout << "SID:" << station_id << std::endl;
        struct timespec now_timespec;
        int ret = clock_gettime(CLOCK_MONOTONIC, &now_timespec);
        assert(ret == 0);
        uint64_t now = now_timespec.tv_sec;

        auto iter = this->last_inserted_times.find(station_id);

        // First time we've seen this station.  Allow insert.
        if (iter == this->last_inserted_times.end()) {
            last_inserted_times[station_id] = now;
            return true;
        }

        // Otherwise check if it time to insert.
        uint64_t last_inserted_time = iter->second;
        if (now - last_inserted_time >= throttle_time) {
            last_inserted_times[station_id] = now;
            return true;
        }

        return false;
    }

};


int main()
{
    Throttler throttler;

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a response socket and bind to interface
    zmq::socket_t socket{context, ZMQ_PULL};
    // FIXME move to common or make configurable.
    socket.bind("tcp://*:5556");

    for (;;) {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);

        std::cout << "SQL server received " << std::endl;
        zweather::WeatherData data_point;

        std::string data(reinterpret_cast<const char*>(request.data()));
        data_point.ParseFromString(data);

        if (throttler.can_insert(data_point.station_id())) {
            std::cout << "Inserted in SQL\n";
        }
        std::cout << data_point << "\n\n";

        // FIXME remove
       // zmq::message_t reply;
       // socket.send(reply);
    }

    return 0;
}
