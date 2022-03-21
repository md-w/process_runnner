# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation Util)

add_library(pipeline_remote STATIC
    src/pipeline_remote.cpp
)

target_precompile_headers(pipeline_remote
    PUBLIC inc/pipeline_remote.h
)

target_include_directories(pipeline_remote
    PUBLIC inc
)

target_link_libraries(pipeline_remote
    PRIVATE vlogger
    PRIVATE Poco::Foundation
    PRIVATE Poco::Util
)

set_property(TARGET pipeline_remote PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")