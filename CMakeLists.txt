# Copyright 2021 the gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ timetest example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building timetest.

cmake_minimum_required(VERSION 3.5.1)

project(timetest C CXX)

include(../wi-AFS/common.cmake)

# Proto file
get_filename_component(timetest_proto "protos/timetest.proto" ABSOLUTE)
get_filename_component(timetest_proto_path "${timetest_proto}" PATH)

# Generated sources
set(timetest_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/timetest.pb.cc")
set(timetest_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/timetest.pb.h")
set(timetest_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/timetest.grpc.pb.cc")
set(timetest_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/timetest.grpc.pb.h")
add_custom_command(
      OUTPUT "${timetest_proto_srcs}" "${timetest_proto_hdrs}" "${timetest_grpc_srcs}" "${timetest_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${timetest_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${timetest_proto}"
      DEPENDS "${timetest_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# timetest_grpc_proto
add_library(timetest_grpc_proto
  ${timetest_grpc_srcs}
  ${timetest_grpc_hdrs}
  ${timetest_proto_srcs}
  ${timetest_proto_hdrs})
target_link_libraries(timetest_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# client
add_executable(client "client.cc")
target_link_libraries(client
  timetest_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# server
add_executable(server "server.cc")
target_link_libraries(server
  timetest_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
