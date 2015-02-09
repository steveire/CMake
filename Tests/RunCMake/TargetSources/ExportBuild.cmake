add_library(iface INTERFACE)
target_sources(iface INTERFACE "empty_1.cpp")

export(TARGETS iface FILE ${CMAKE_CURRENT_BINARY_DIR}/targets.cmake)
