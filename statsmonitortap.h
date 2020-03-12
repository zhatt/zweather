/*
 * StatsMonitorTap class
 *
 * Monitoring endpoint.  Current implementation is used to keep track
 * of bytes transmitted and received.
 *
 * FIXME figure out how to make this more generic my dynamically creating an
 * array of counters.
 */

#ifndef STATSMONITORTAP_H
#define STATSMONITORTAP_H

#include <zmq.hpp>

class StatsMonitorTap {

    zmq::socket_t tap_socket;

    unsigned long bytes_received = 0;
    unsigned long bytes_sent = 0;

  public:

    // Create a listener in the zmq::context.
    StatsMonitorTap( zmq::context_t& zmq_context,
                     const std::string& listener_port );

    // Add to counters.
    void add_bytes_received( unsigned long bytes );
    void add_bytes_sent( unsigned long bytes );

    // Service the tap.  The tap runs synchonously so you need to call this
    // periodically to service the tap port.
    void service_tap();
};

#endif // STATSMONITORTAP_H
