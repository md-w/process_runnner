# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(gRPC REQUIRED)

add_library(process_runner_client
    src/process_runner_client.cpp
)

target_include_directories(process_runner_client
    PUBLIC inc
)

target_link_libraries(process_runner_client
    PRIVATE gRPC::grpc++
    PRIVATE vlogger
    PRIVATE data_models
)

set_property(TARGET process_runner_client PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")