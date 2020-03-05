#ifndef STATSMONITORTAP_H
#define STATSMONITORTAP_H

#include <zmq.hpp>

class StatsMonitorTap {

    zmq::socket_t tap_socket;

    unsigned long bytes_received = 0;
    unsigned long bytes_sent = 0;

  public:

    StatsMonitorTap( zmq::context_t& zmq_context,
                     const std::string& listener_port );

    void add_bytes_received( unsigned long bytes );
    void add_bytes_sent( unsigned long bytes );

    void service_tap();
};

#endif // STATSMONITORTAP_H
