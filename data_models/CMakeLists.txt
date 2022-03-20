# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required (VERSION 3.20)

add_library(data_models
    src/data_models.cpp
)

target_precompile_headers(data_models
  PUBLIC inc/data_models.h
)

target_include_directories(data_models
    PUBLIC inc
)

set_property(TARGET data_models PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
