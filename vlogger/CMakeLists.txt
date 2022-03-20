# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required (VERSION 3.20)

find_package(spdlog REQUIRED)

add_library(vlogger
    src/Chameleon.cpp
    src/ConfigFile.cpp
    src/logging.cpp
)

target_precompile_headers(vlogger
  PUBLIC inc/logging.h
  PUBLIC inc/Chameleon.h
  PUBLIC inc/ConfigFile.h
)

target_include_directories(vlogger
    PUBLIC inc
)

target_link_libraries(vlogger
  PUBLIC spdlog::spdlog
)

set_property(TARGET vlogger PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#[[
add_executable (vloggertest src/vloggertest.cpp)
target_link_libraries (vloggertest vlogger)
#]]
