# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required (VERSION 3.20)

find_package(gRPC REQUIRED) # use protobuf implicitly

#
# Compile protobuf and grpc files in myproto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)


# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/process_runner.proto)

set(PROTO_FILES
    interfaces/process_runner.proto
)

add_library(data_models_protobuf_helper STATIC
    src/protobuf_helper.cpp
)

target_precompile_headers(data_models_protobuf_helper
  PUBLIC inc/protobuf_helper.h
)

target_include_directories(data_models_protobuf_helper
  PUBLIC inc
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(data_models_protobuf_helper
  PUBLIC gRPC::grpc
  PUBLIC gRPC::grpc++
)

protobuf_generate(TARGET data_models_protobuf_helper
  PROTOS ${PROTO_FILES}
  LANGUAGE cpp
)
protobuf_generate(TARGET data_models_protobuf_helper
  PROTOS ${PROTO_FILES}
  LANGUAGE grpc
  GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
  PLUGIN protoc-gen-grpc=${grpc_cpp_plugin_location}
)

set_target_properties(
  data_models_protobuf_helper
    PROPERTIES
         CXX_CLANG_TIDY ""
)

add_library(data_models
  src/data_models.cpp
)

target_precompile_headers(data_models
  PUBLIC inc/data_models.h
)

target_include_directories(data_models
  PUBLIC inc
)

target_link_libraries(data_models
  PUBLIC data_models_protobuf_helper
)

set_property(TARGET data_models_protobuf_helper PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET data_models PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
