# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Catch2 REQUIRED)
add_executable(tests
    tests.cpp
)
target_link_libraries(tests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE data_models
    PRIVATE process_runner_service
)

set_property(TARGET tests PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")