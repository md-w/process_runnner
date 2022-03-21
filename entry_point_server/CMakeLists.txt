# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Util)

add_executable(entry_point_server
    src/entry_point.cpp
    src/globalerrorhandler.cpp
)

target_include_directories(entry_point_server
    PRIVATE inc
    PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(entry_point_server
    PRIVATE Poco::Util
    PRIVATE vlogger
    PRIVATE vutilities
    PRIVATE data_models
    PRIVATE process_runner
    PRIVATE process_runner_service
)
set_property(TARGET entry_point_server PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
