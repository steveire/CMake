
cmake_minimum_required(VERSION 2.8)

project(HostAndTarget)

add_executable(host_tool host_tool.cpp)
set_property(TARGET host_tool APPEND PROPERTY BUILD_TOOLCHAINS HOST)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/host_generated.h"
  COMMAND host_tool ARGS "${CMAKE_BINARY_DIR}/host_generated.h" "HOST_DEFINE"
)

macro(add_target_library name toolchain)
  add_library(${name}${toolchain} targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
  target_include_directories(${name}${toolchain} PRIVATE "${CMAKE_BINARY_DIR}")
  set_property(TARGET ${name}${toolchain} APPEND PROPERTY BUILD_TOOLCHAINS ${toolchain})
endmacro()

add_target_library(targetlib RaspPi)
add_target_library(targetlib Android14Mips)
add_target_library(targetlib QNX)
add_target_library(targetlib ClangArm)
