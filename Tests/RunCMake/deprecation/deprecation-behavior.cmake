
cmake_minimum_required(VERSION 2.8)
project(deprecation-behavior)

unset(tc_result CACHE)
try_compile(tc_result
  "${CMAKE_CURRENT_BINARY_DIR}/binary"
  "${CMAKE_CURRENT_SOURCE_DIR}/source"
  deprecation-test
  OUTPUT_VARIABLE tc_output
)

if (NOT tc_result)
  message(SEND_ERROR "Fail!")
endif()

message(${tc_output})
