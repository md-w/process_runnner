# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation Util)

add_library(pipeline STATIC
    src/pipeline.cpp
)

target_precompile_headers(pipeline
    PUBLIC inc/pipeline.h
)

target_include_directories(pipeline
    PUBLIC inc
)

target_link_libraries(pipeline
    PUBLIC vlogger
    PRIVATE Poco::Foundation
    PRIVATE Poco::Util
)

set_property(TARGET pipeline PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")