
PROTO_PATH = .

CXXFLAGS = -Wall -Werror -Og -g -Wdeprecated-declarations
# FIXME figure out what ZeroMQ deprecated in its API and remove this flag.
CXXFLAGS += -Wno-deprecated-declarations
CXXFLAGS += -I/usr/include/google/protobuf -I/usr/include/google

LDFLAGS = -lstdc++ -lprotobuf -lzmq

PROGS += weather_station
PROGS += weather_edge_server
PROGS += weather_sql_store
PROGS += weather_raw_store

# Object files that everything needs
SHARED_OBJ += base64string.o
SHARED_OBJ += hexstring.o
SHARED_OBJ += rawbytes.o
SHARED_OBJ += tune.o
SHARED_OBJ += weather_common.o
SHARED_OBJ += weather_data.pb.o

# Extra object files that tests need
SHARED_OBJ_TEST += testmain.o

PROTOBUFS +=  weather_data.pb.h weather_data.pb.cc weather_data_pb2.py

TESTPROGS += base64string-test
TESTPROGS += hexstring-test
TESTPROGS += rawbytes-test
TESTPROGS += tune-test

all: ${PROGS} ${TESTPROGS} ${PROTOBUFS}

test: $(TESTPROGS:-test=-test.out)

weather_station: weather_station.o ${SHARED_OBJ}

weather_edge_server: weather_edge_server.o ${SHARED_OBJ}

weather_sql_store: weather_sql_store.o ${SHARED_OBJ}

weather_raw_store: weather_raw_store.o ${SHARED_OBJ}


# FIXME protoc supports automatic dependency generation.
# FIXME after make clean this dependency doesn't work for make all,
# you need to run make weather_data.pb.h to trigger it the first time.
weather_data.pb.h weather_data.pb.cc weather_data_pb2.py: weather_data.proto
	protoc \
		--proto_path=${PROTO_PATH} \
		--cpp_out=$$PWD \
		--python_out=$$PWD \
		weather_data.proto

test: $(TESTPROGS:-test=-test.out)

base64string-test: base64string-test.o ${SHARED_OBJ} ${SHARED_OBJ_TEST} -lgtest
hexstring-test: hexstring-test.o ${SHARED_OBJ} ${SHARED_OBJ_TEST} -lgtest
rawbytes-test: rawbytes-test.o ${SHARED_OBJ} ${SHARED_OBJ_TEST} -lgtest
tune-test: tune-test.o ${SHARED_OBJ} ${SHARED_OBJ_TEST} -lgtest

clean:
	rm -f ${PROGS} ${PROTOBUFS} ${SHARED_OBJ} ${TESTPROGS} *.o ${DEPDIR}/* *-test.out

%-test.out: %-test
	set -o pipefail; \
	"./$<" | tee "$@.tmp"; \
	[[ $$? -eq 0 ]] && mv "$@.tmp" "$@" || exit 1

include Makefile.inc
