
cmake_minimum_required(VERSION 2.8)

project(target)

include("${CMAKE_BINARY_DIR}/../../../host-prefix/src/host-build/HostTargets.cmake")

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/host_generated.h"
  COMMAND HOST::host_tool ARGS "${CMAKE_BINARY_DIR}/host_generated.h" "HOST_DEFINE"
)

include_directories("${CMAKE_BINARY_DIR}")
add_library(targetlib ../targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
