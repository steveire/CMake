cmake_minimum_required(VERSION 3.1)
project(autorcc_depends)

set(CMAKE_AUTORCC ON)

if (QT_TEST_VERSION STREQUAL 4)
  find_package(Qt4 REQUIRED)
  set(QT_CORE_TARGET Qt4::QtCore)
else()
  if (NOT QT_TEST_VERSION STREQUAL 5)
    message(SEND_ERROR "Invalid Qt version specified.")
  endif()

  find_package(Qt5Core REQUIRED)
  set(QT_CORE_TARGET Qt5::Core)
endif()

configure_file(res1.qrc.in res1.qrc @ONLY)
configure_file(res1/input.txt.in res1/input.txt @ONLY)

add_executable(test_res1
  test_res1.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/res1.qrc
)
target_link_libraries(test_res1 ${QT_CORE_TARGET})
add_custom_command(TARGET test_res1 POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:test_res1>\n$<TARGET_FILE:test_res2>\n" > info_file.txt)

add_custom_command(OUTPUT generated.txt
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/res2_input.txt" "${CMAKE_CURRENT_BINARY_DIR}/generated.txt"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res2_input.txt"
)

configure_file(res2.qrc.in res2_gen.qrc)

add_executable(test_res2
  test_res2.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/res2_gen.qrc
)
target_link_libraries(test_res2 ${QT_CORE_TARGET})
