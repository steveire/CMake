
cmake_minimum_required(VERSION 2.8)

project(HostAndTarget)

add_executable(host_tool host_tool.cpp)
set_property(TARGET host_tool APPEND PROPERTY BUILD_TOOLCHAINS HOST)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/host_generated.h"
  COMMAND host_tool ARGS "${CMAKE_BINARY_DIR}/host_generated.h" "HOST_DEFINE"
)

add_executable(build_tool build_tool.cpp)
set_property(TARGET build_tool APPEND PROPERTY BUILD_TOOLCHAINS TARGET) # Should be HOST when used.

add_library(bootstraplib bootstraplib.cpp)
set_property(TARGET bootstraplib APPEND PROPERTY BUILD_TOOLCHAINS TARGET) # Should be HOST when used.

target_link_libraries(build_tool bootstraplib)

export(TARGETS build_tool bootstraplib)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/build_generated.h"
  COMMAND build_tool ARGS "${CMAKE_BINARY_DIR}/build_generated.h" "BUILD_DEFINE"
)

add_library(targetlib targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")
target_include_directories(targetlib PRIVATE "${CMAKE_BINARY_DIR}")
set_property(TARGET targetlib APPEND PROPERTY BUILD_TOOLCHAINS TARGET)
