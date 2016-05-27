cmake_minimum_required(VERSION 3.4)

project(ServerTests)

macro(do_test bsname file)
  execute_process(COMMAND python
      "${CMAKE_SOURCE_DIR}/server-test.py"
      "${CMAKE_COMMAND}"
      "${CMAKE_SOURCE_DIR}/${file}"
      "${CMAKE_SOURCE_DIR}"
      "${CMAKE_BINARY_DIR}"
    RESULT_VARIABLE test_result)

  if (NOT test_result EQUAL 0)
    message(SEND_ERROR "TEST FAILED")
  endif()
endmacro()

do_test("test_handshake" "tc_handshake.json")
do_test("test_globalSettings" "tc_globalSettings.json")

add_executable(Server empty.cpp)
