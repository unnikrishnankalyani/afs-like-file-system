LDFLAGS = -L/usr/local/lib -L/users/monshiva/fuse-3.10.5/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -lfuse

CXX = g++
CPPFLAGS += -I/usr/local/include -I/users/monshiva/fuse-3.10.5/include -pthread 
CXXFLAGS += -std=c++11 -D_FILE_OFFSET_BITS=64

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = /protos

vpath %.proto $(PROTOS_PATH)

all: client server

client: afs.pb.o afs.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

server: afs.pb.o afs.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --cpp_out=. $<



clean:
	rm -f *.o *.pb.cc *.pb.h client server
