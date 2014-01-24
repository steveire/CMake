cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(DBusTest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOQDBUS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)

if (QT_TEST_QT5_EXE)
  find_package(Qt5DBus REQUIRED)
  find_package(Qt5Widgets REQUIRED)
endif()

add_executable(publisher publisher.cpp)
set_property(TARGET publisher PROPERTY AUTOQDBUS_OPTIONS -A)
if (QT_TEST_QT5_EXE)
  target_link_libraries(publisher Qt5::Widgets Qt5::DBus)
else()
  target_link_libraries(publisher Qt4::QtGui Qt4::QtDBus)
endif()

add_executable(subscriber subscriber.cpp)
target_link_libraries(subscriber Qt4::QtGui Qt4::QtDBus)
