# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(gRPC REQUIRED)
find_package(Poco REQUIRED Util)
add_library(process_runner_service
    src/process_runner_service.cpp
    src/process_runner_service_run.cpp
)

target_include_directories(process_runner_service
    PUBLIC inc
)

target_link_libraries(process_runner_service
    PRIVATE gRPC::grpc++
    PRIVATE vlogger
    PRIVATE vutilities
    PRIVATE data_models
    PRIVATE process_runner
    PRIVATE Poco::Util
)

set_property(TARGET process_runner_service PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")