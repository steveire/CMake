
cmake_minimum_required (VERSION 2.8)

project(ExternalProjects)

include(ExternalProject)

ExternalProject_Add(host
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/host"
  INSTALL_COMMAND ""
)

set(TARGET_CMAKE_ARGS)
if (CMAKE_TOOLCHAIN_FILE)
  set(TARGET_CMAKE_ARGS CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()

ExternalProject_Add(target
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/target"
  DEPENDS host
  INSTALL_COMMAND ""
  ${TARGET_CMAKE_ARGS}
)
