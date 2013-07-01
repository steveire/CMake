
set(CMAKE_SYSTEM_NAME Linux)

set(prefix "/usr")

set(CMAKE_C_COMPILER "${prefix}/bin/clang")
set(CMAKE_CXX_COMPILER "${prefix}/bin/clang++")
set(triple arm-linux-gnueabihf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# Clang workarounds
set(toolchain_lib_dir_1 "/usr/lib/gcc-cross/${triple}/4.7")
set(toolchain_lib_dir_2 "/usr/${triple}/lib/")
set(CMAKE_TOOLCHAIN_LINK_FLAGS "-B${toolchain_lib_dir_1} -B${toolchain_lib_dir_2} -L${toolchain_lib_dir_1} -L${toolchain_lib_dir_2}")

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_TOOLCHAIN_LINK_FLAGS} CACHE INTERNAL "exe link flags")
set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_TOOLCHAIN_LINK_FLAGS} CACHE INTERNAL "module link flags")
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_TOOLCHAIN_LINK_FLAGS} CACHE INTERNAL "shared link flags")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
