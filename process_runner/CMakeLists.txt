# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation Util)

add_library(process_runner STATIC
    src/process_runner.cpp
)

target_precompile_headers(process_runner
    PUBLIC inc/process_runner.h
)

target_include_directories(process_runner
    PUBLIC inc
)

target_link_libraries(process_runner
    PUBLIC data_models
    PRIVATE vlogger
    PRIVATE Poco::Foundation
    PRIVATE Poco::Util
)

set_property(TARGET process_runner PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")