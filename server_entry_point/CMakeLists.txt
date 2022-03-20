# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(gRPC REQUIRED)
find_package(Poco REQUIRED Util)

add_executable(server_entry_point
    src/entry_point.cpp
    src/globalerrorhandler.cpp
)

target_include_directories(server_entry_point
    PRIVATE inc
    PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(server_entry_point
    PRIVATE Poco::Util
    PRIVATE gRPC::grpc++
    PRIVATE vlogger
    PRIVATE vutilities
    PRIVATE data_models
)
set_property(TARGET server_entry_point PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
