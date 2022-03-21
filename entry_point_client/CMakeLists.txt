# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(gRPC REQUIRED)
find_package(Poco REQUIRED Util)

add_executable(entry_point_client
    src/entry_point.cpp
    src/globalerrorhandler.cpp
)

target_include_directories(entry_point_client
    PRIVATE inc
    PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(entry_point_client
    PRIVATE Poco::Util
    PRIVATE gRPC::grpc++
    PRIVATE vlogger
    PRIVATE vutilities
    PRIVATE data_models
    PRIVATE process_runner_client
)

set_property(TARGET entry_point_client PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")