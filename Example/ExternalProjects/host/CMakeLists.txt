
cmake_minimum_required(VERSION 2.8)

project(host)

add_executable(host_tool ../host_tool.cpp)

export(TARGETS host_tool NAMESPACE HOST:: FILE HostTargets.cmake)
