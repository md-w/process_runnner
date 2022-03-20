# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(absl REQUIRED)

add_library(vutilities
    src/utilities.cpp
    src/sys_timestamp.cpp
)

target_include_directories(vutilities
    PUBLIC inc
)

target_link_libraries(vutilities
    PRIVATE absl::strings
)



if (MSVC)
else()
target_link_libraries(vutilities
	PUBLIC stdc++fs
)
endif (MSVC)

set_property(TARGET vutilities PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
