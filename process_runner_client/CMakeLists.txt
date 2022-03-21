# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation Util)

add_library(process_runner_client STATIC
    src/process_runner_client.cpp
)

target_precompile_headers(process_runner_client
    PUBLIC inc/process_runner_client.h
)

target_include_directories(process_runner_client
    PUBLIC inc
)

target_link_libraries(process_runner_client
    PRIVATE vlogger
    PRIVATE Poco::Foundation
    PRIVATE Poco::Util
)

set_property(TARGET process_runner_client PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")