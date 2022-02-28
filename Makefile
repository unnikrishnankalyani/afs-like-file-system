LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl

FUSE_PATH = ../fuse-3.10.5
CXX = g++
CFLAGS += -DHAVE_CONFIG_H -D_FILE_OFFSET_BITS=64 -I$(FUSE_PATH)/include -I. -Wall -g -O3

CPPFLAGS += `pkg-config /usr/include/fuse --cflags protobuf grpc --libs`
CXXFLAGS += -std=c++11 -D_FILE_OFFSET_BITS=64

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
LIBTOOL =  $(FUSE_PATH)/libtool
LIBTOOLFLAGS = --silent --mode=link
PROTOS_PATH = /protos

FUSELIB = $(FUSE_PATH)/lib/libfuse.la $(FUSE_PATH)/lib/libulockmgr.la

vpath %.proto $(PROTOS_PATH)

all: client server

client: afs.pb.o afs.grpc.pb.o client.o 
	$(LIBTOOL) $(LIBTOOLFLAGS) $(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@ $(FUSELIB)
server: afs.pb.o afs.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --cpp_out=. $<



clean:
	rm -f *.o *.pb.cc *.pb.h client server
