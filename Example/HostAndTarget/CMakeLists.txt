
cmake_minimum_required(VERSION 2.8)

project(HostAndTarget)

add_executable(host_tool host_tool.cpp)
set_property(TARGET host_tool APPEND PROPERTY BUILD_TOOLCHAINS HOST)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/host_generated.h"
  COMMAND host_tool ARGS "${CMAKE_BINARY_DIR}/host_generated.h" "HOST_DEFINE"
)

add_library(targetlib targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
target_include_directories(targetlib PRIVATE "${CMAKE_BINARY_DIR}")
set_property(TARGET targetlib APPEND PROPERTY BUILD_TOOLCHAINS TARGET)

add_library(rpitargetlib targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
target_include_directories(rpitargetlib PRIVATE "${CMAKE_BINARY_DIR}")
set_property(TARGET rpitargetlib APPEND PROPERTY BUILD_TOOLCHAINS RaspPi)

add_library(androidtargetlib targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
target_include_directories(androidtargetlib PRIVATE "${CMAKE_BINARY_DIR}")
set_property(TARGET androidtargetlib APPEND PROPERTY BUILD_TOOLCHAINS Android14Mips)
