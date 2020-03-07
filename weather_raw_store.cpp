#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <zmq.hpp>

#include "base64string.h"
#include "statsmonitortap.h"
#include "tune.h"
#include "weather_common.h"
#include "weather_data.pb.h"


static Tune
setup_tuning_variables() {
    Tune tune( "WEATHER_RAW_STORE_" );

    tune.add_variable( "LISTEN_PORT", "5557" );
    tune.add_variable( "TAP_PORT", "6557" );

    tune.add_variable( "LOG_PATH_PREFIX", "rawlogs/" );
    tune.add_variable( "LOG_ROTATION_SIZE", "10000" );

    return tune;
}

// Generate a log time.  Format is raw--timestamp.log.
// Example:  raw--2020-03-02T20:10:20.log
//
static std::string
generate_log_name(const std::string& prefix) {
    struct timeval tv;
    time_t nowtime;
    struct tm nowtm;
    char tmbuf[40];  // Holds 2020-03-02T20:10:20

    int rc = gettimeofday(&tv, NULL);
    if (rc != 0) {
        throw( std::runtime_error( "gettimeofday failed" ) );
    }

    nowtime = tv.tv_sec;
    void* err = localtime_r(&nowtime, &nowtm);
    if (err != &nowtm) {
        throw( std::runtime_error( "localtime failed" ) );
    }

    rc = strftime(tmbuf, sizeof tmbuf, "%Y-%m-%dT%H:%M:%S", &nowtm);
    if (rc == 0) {
        throw( std::runtime_error( "strftime failed" ) );
    }

    std::string log_name( prefix + "raw--" + std::string(tmbuf) + ".log" );
    return log_name;
}

class RawLogWriter {
    unsigned long bytes_written = 0;
    unsigned long rotation_size;
    int fd = -1;
    std::string prefix;  // i.e. directory/ <- Be sure add the /.

  public:
    RawLogWriter( const std::string& prefix, uint64_t rotation_size ) :
        rotation_size( rotation_size ),
        prefix( prefix ) {}

    void open_log() {
        std::string log_name = generate_log_name( prefix );
        fd = open(log_name.c_str(), O_CREAT|O_EXCL|O_WRONLY, 0777);
        if (fd == -1) {
            throw( std::runtime_error( "log file open failed" ) );
        }
    }

    void write_line(const std::string& line){
        if (bytes_written > rotation_size) {
            int rc = fsync(fd);
            if (rc == -1) {
                throw( std::runtime_error( "log file fsync failed" ) );
            }

            rc = close(fd);
            if (rc == -1) {
                throw( std::runtime_error( "log file close failed" ) );
            }

            fd = -1;
            bytes_written = 0;
        }

        if (fd == -1) {
            open_log();
        }

        int bytes_sent = 0;
        int remaining = line.size();
        const char* buffer = line.c_str();
        while (remaining) {
            bytes_sent = write(fd, buffer, remaining);

            remaining -= bytes_sent;
            buffer += bytes_sent;
        }

        bytes_written += line.size();
    }
};

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Starting raw store" << std::endl;

    Tune tune = setup_tuning_variables();

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a response socket and bind to interface
    zmq::socket_t socket{context, ZMQ_PULL};
    socket.bind( "tcp://*:" + tune.get( "LISTEN_PORT" ) );

    RawLogWriter raw_log( tune.get( "LOG_PATH_PREFIX"),
                          std::stoul( tune.get( "LOG_ROTATION_SIZE" ) ) );

    StatsMonitorTap stats_tap( context,
                               std::string( tune.get( "TAP_PORT" ) ) );

    for (;;) {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);
        stats_tap.add_bytes_received( request.size() );

        // convert to protobuf.
        zweather::WeatherData data_point;
        std::string data(reinterpret_cast<const char*>(request.data()));
        data_point.ParseFromString(data);

        // serialize to base64.
        std::ostringstream log_line;
        log_line << data_point.station_id() << " "
                 << ProtoBufToBase64(data_point) << "\n";

        // write to log.
        std::cout << log_line.str() << std::flush;
        raw_log.write_line(log_line.str());

        stats_tap.service_tap();
    }

    return 0;
}
