
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

PROTOBUFS +=  weather_data.pb.h weather_data.pb.cc weather_data_pb2.py

TESTPROGS = ${PROGS:=-test}
TESTPROGS += machine-test

# FIXME add TESTPROGS
#all: ${PROGS} ${TESTPROGS} ${PROTOBUFS}
all: ${PROGS} ${PROTOBUFS}

weather_station: weather_station.o weather_data.pb.o weather_common.o

weather_edge_server: weather_edge_server.o weather_data.pb.o weather_common.o

weather_sql_store: weather_sql_store.o weather_data.pb.o weather_common.o

weather_raw_store: weather_raw_store.o weather_data.pb.o weather_common.o


# FIXME protoc supports automatic dependency generation.
weather_data.pb.h weather_data.pb.cc weather_data_pb2.py: weather_data.proto
	protoc \
		--proto_path=${PROTO_PATH} \
		--cpp_out=$$PWD \
		--python_out=$$PWD \
		weather_data.proto

test: $(TESTPROGS:-test=-test.out)

clean:
	rm -f ${PROGS} ${PROTOBUFS} ${TESTPROGS} *.o ${DEPDIR}/* *-test.out

%-test.out: %-test
	set -o pipefail; \
	"./$<" | tee "$@.tmp"; \
	[[ $$? -eq 0 ]] && mv "$@.tmp" "$@" || exit 1

include Makefile.inc
