
cmake_minimum_required(VERSION 2.8)

project(Qt5Dependent)

toolchains(HOST)

  find_package(Qt5Core REQUIRED) # Finds Qt5 on the host.

  add_executable(host_tool host_tool.cpp)
  # No need to set the BUILD_TOOLCHAINS property.
  target_link_libraries(host_tool Qt5::Core)

  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/host_generated.h"
    COMMAND host_tool ARGS "${CMAKE_BINARY_DIR}/host_generated.h" "HOST_DEFINE"
  ) # Custom command only run once.
endtoolchains()

# Build targetlib for each of these
toolchains(RaspPi QNX ClangArm Android14Mips)

  find_package(Qt5Gui REQUIRED) # Finds Qt5 on each target

  # Only one library target name given to cmake. Don't encode the toolchain.
  add_library(targetlib targetlib.cpp "${CMAKE_BINARY_DIR}/host_generated.h")

  target_include_directories(targetlib PRIVATE "${CMAKE_BINARY_DIR}")
  target_link_libraries(targetlib Qt5::Gui)

endtoolchains()

# Open questions:
#  Can generators handle this?
#  What libraries should be built by 'make targetlib'?
#  Should multiple per-toolchain make targets be generated for targetlib?
