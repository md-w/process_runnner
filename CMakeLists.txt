# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required(VERSION 3.20)
project(process_runnner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Git)
if(Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --abbrev=40
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_HASH)
    message(STATUS gch: ${GIT_COMMIT_HASH})
    #add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
    set(BUILD_NUMBER ${GIT_COMMIT_HASH})

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_BRANCH)
    message(STATUS gbr: ${GIT_COMMIT_BRANCH})
    #add_definitions(-DGIT_COMMIT_BRANCH="${GIT_COMMIT_BRANCH}")
endif() #git
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;)
endif()

set(APPLICATION_NAME "process_runner")
set(APPLICATION_GUID "0tDe3o5UiUODZSmfmwl32Q")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(VENDOR_NAME "md-w")

configure_file (
    "${PROJECT_SOURCE_DIR}/Version.h.in"
    "${PROJECT_BINARY_DIR}/Version.h"
)

configure_file (
    "${PROJECT_SOURCE_DIR}/Application.h.in"
    "${PROJECT_BINARY_DIR}/Application.h"
)

add_subdirectory(vlogger)
add_subdirectory(vutilities)
add_subdirectory(data_models)
add_subdirectory(process_runner_service)
add_subdirectory(process_runner)
add_subdirectory(process_runner_client)
add_subdirectory(entry_point_server)
add_subdirectory(entry_point_client)
add_subdirectory(tests)