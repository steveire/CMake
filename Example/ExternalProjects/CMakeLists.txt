
cmake_minimum_required (VERSION 2.8)

project(ExternalProjects)

include(ExternalProject)

ExternalProject_Add(host
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/host"
  INSTALL_COMMAND ""
)

ExternalProject_Add(target
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/target"
  DEPENDS host
  INSTALL_COMMAND ""
)
