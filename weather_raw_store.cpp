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
#include "weather_common.h"
#include "weather_data.pb.h"

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
        std::cerr << "gettimeofday failed" << std::endl;
        exit(1);
    }

    nowtime = tv.tv_sec;
    void* err = localtime_r(&nowtime, &nowtm);
    if (err != &nowtm) {
        std::cerr << "localtime failed" << std::endl;
        exit(1);
    }

    rc = strftime(tmbuf, sizeof tmbuf, "%Y-%m-%dT%H:%M:%S", &nowtm);
    if (rc == 0) {
        std::cerr << "strftime failed" << std::endl;
        exit(1);
    }

    std::string log_name( prefix + "raw--" + std::string(tmbuf) + ".log" );
    return log_name;
}

class RawLogWriter {
    uint64_t bytes_written = 0;
    // FIXME parameterize
    uint64_t rotation_size = 1000;
    int fd = -1;
    std::string prefix;  // i.e. directory/ <- Be sure add the /.

  public:
    RawLogWriter(const std::string& prefix) :
        prefix( prefix ) {}

    void open_log() {
        // FIXME parameterize
        std::string log_name = generate_log_name("rawlogs/");
        fd = open(log_name.c_str(), O_CREAT|O_EXCL|O_WRONLY, 0777);
        if (fd == -1) {
            std::cerr << "log file open failed" << std::endl;
            exit(1);
        }
    }

    void write_line(const std::string& line){
        if (bytes_written > rotation_size) {
            int rc = fsync(fd);
            if (rc == -1) {
                std::cerr << "log file fsync failed" << std::endl;
                exit(1);
            }

            rc = close(fd);
            if (rc == -1) {
                std::cerr << "log file close failed" << std::endl;
                exit(1);
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
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a response socket and bind to interface
    zmq::socket_t socket{context, ZMQ_PULL};
    // FIXME move to common or make configurable.
    socket.bind("tcp://*:5557");

    RawLogWriter raw_log("rawlogs/");

    for (;;) {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);

        zweather::WeatherData data_point;
        std::string data(reinterpret_cast<const char*>(request.data()));
        data_point.ParseFromString(data);

        std::ostringstream log_line;
        log_line << data_point.station_id() << " "
                 << ProtoBufToBase64(data_point) << "\n";

        std::cout << log_line.str();
        raw_log.write_line(log_line.str());
    }

    return 0;
}
