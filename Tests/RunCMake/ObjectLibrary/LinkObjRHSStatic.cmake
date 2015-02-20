project(LinkObjRHSStatic C)

add_library(AnObjLib OBJECT a.c)

add_library(A SHARED b.c $<TARGET_OBJECTS:AnObjLib>)
target_link_libraries(A AnObjLib)

add_executable(exe exe.c)
target_link_libraries(exe A)
