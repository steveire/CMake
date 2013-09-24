cmake_minimum_required(VERSION 2.8)

project(deprecation-test)

include(GenerateExportHeader)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(testlib lib.cpp)
generate_export_header(testlib)
