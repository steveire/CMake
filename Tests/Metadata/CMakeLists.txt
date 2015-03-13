
cmake_minimum_required(VERSION 3.2)

project(Metadata)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

add_library(iface_dep INTERFACE)
target_sources(iface_dep INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/iface_dep.cpp")
target_compile_definitions(iface_dep INTERFACE iface_DEF)

add_library(static_dep STATIC static_dep.cpp)
target_compile_definitions(static_dep
  PUBLIC STATIC_PUBLIC
  PRIVATE STATIC_PRIVATE
)
target_include_directories(static_dep
  PUBLIC static_public_dir
  PRIVATE static_private_dir
)
target_sources(static_dep
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/static_iface.cpp"
)

add_library(shared_dep SHARED shared_dep.cpp)
target_compile_definitions(shared_dep
  PUBLIC SHARED_PUBLIC
  PRIVATE SHARED_PRIVATE
)
target_include_directories(shared_dep
  PUBLIC shared_public_dir
  PRIVATE shared_private_dir
)

add_library(obj_dep OBJECT obj_dep.cpp)

set(MSVC_DEBUG_WIN $<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>)

set_source_files_properties(main.cpp
  PROPERTIES
    COMPILE_FLAGS "-DTHIS_IS_THE_MAIN_FILE"
    COMPILE_DEFINITIONS "THIS_IS_THE_MAIN_FILE_DEF"
)

set_source_files_properties(compiled_as_cxx.c PROPERTIES LANGUAGE CXX)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated_source.cpp" "${CMAKE_CURRENT_BINARY_DIR}/generated_source.h"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/input_source.cpp"
                                   "${CMAKE_CURRENT_BINARY_DIR}/generated_source.cpp"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/input_source.h"
                                   "${CMAKE_CURRENT_BINARY_DIR}/generated_source.h"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/input_source.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/input_source.h"
)

add_executable(main
  # Object sources CXX
  main.cpp
  bar_file.cpp
  $<TARGET_OBJECTS:obj_dep>

  # Header sources
  bar_file.h

  # Generated sources
  generated_source.cpp
  generated_source.h

  # Either object sources or excluded sources, depending on platform and config
  $<$<CONFIG:Debug>:foo_debug.cpp>
  $<$<CONFIG:Release>:foo_release.cpp>
  $<$<NOT:$<CONFIG:Debug>>:foo_not_debug.cpp>
  $<$<NOT:$<CONFIG:Release>>:foo_not_release.cpp>
  $<$<PLATFORM_ID:Linux>:foo_linux.cpp>
  $<$<PLATFORM_ID:Windows>:foo_windows.cpp>
  $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Linux>>:foo_windows_or_linux.cpp>
  $<$<PLATFORM_ID:Linux>:$<$<CXX_COMPILER_ID:GNU>:foo_linux_gcc.cpp>>
  $<$<PLATFORM_ID:Linux>:$<$<CXX_COMPILER_ID:Clang>:foo_linux_clang.cpp>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_gt_1800_debug.cpp>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_lt_1800_debug.cpp>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_EQUAL:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_eq_1800_debug.cpp>>
  $<${MSVC_DEBUG_WIN}:$<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,1800>>:foo_msvc_gte_1800_debug.cpp>>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_gt_44.cpp>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_lt_44.cpp>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_EQUAL:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_eq_44.cpp>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,4.4>>>:foo_gcc_gte_44.cpp>

  # Either header sources or excluded sources
  $<$<CONFIG:Debug>:foo_debug.h>
  $<$<CONFIG:Release>:foo_release.h>
  $<$<NOT:$<CONFIG:Debug>>:foo_not_debug.h>
  $<$<NOT:$<CONFIG:Release>>:foo_not_release.h>
  $<$<PLATFORM_ID:Linux>:foo_linux.h>
  $<$<PLATFORM_ID:Windows>:foo_windows.h>
  $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Linux>>:foo_windows_or_linux.h>
  $<$<PLATFORM_ID:Linux>:$<$<CXX_COMPILER_ID:GNU>:foo_linux_gcc.h>>
  $<$<PLATFORM_ID:Linux>:$<$<CXX_COMPILER_ID:Clang>:foo_linux_clang.h>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_gt_1800_debug.h>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_lt_1800_debug.h>>
  $<${MSVC_DEBUG_WIN}:$<$<VERSION_EQUAL:$<CXX_COMPILER_VERSION>,1800>:foo_msvc_eq_1800_debug.h>>
  $<${MSVC_DEBUG_WIN}:$<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,1800>>:foo_msvc_gte_1800_debug.h>>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_gt_44.h>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_lt_44.h>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_EQUAL:$<CXX_COMPILER_VERSION>,4.4>>:foo_gcc_eq_44.h>
  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,4.4>>>:foo_gcc_gte_44.h>

  # Always excluded because of unsatisfiable condition
  $<$<AND:$<CONFIG:Debug>,$<NOT:$<CONFIG:Debug>>>:always_excluded.cpp>

  # Part of Object sources CXX, despite the extension.
  compiled_as_cxx.c

  # Object sources C
  c_file.c

  # Other sources.
  text_file.txt
)
target_compile_definitions(
  main
  PRIVATE
    MAIN_DEF
    PLATFORM_IS_$<PLATFORM_ID>
    CONFIG_IS_$<CONFIG>
    CONFIG_IS_NOT_DEBUG=$<NOT:$<CONFIG:Debug>>
    CONFIG_IS_NOT_RELEASE=$<NOT:$<CONFIG:Release>>
    CXX_COMPILER_IS_$<CXX_COMPILER_ID>
    C_COMPILER_IS_$<C_COMPILER_ID>
    CXX_COMPILER_HAS_VARIADIC_TEMPLATES=$<COMPILE_FEATURES:cxx_variadic_templates>
)
target_include_directories(main
  PRIVATE main_dir
)
target_compile_options(main PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-DCOMPILE_LANGUAGE_CXX>
  $<$<COMPILE_LANGUAGE:C>:-DCOMPILE_LANGUAGE_C>
)

target_link_libraries(main static_dep shared_dep iface_dep)

foreach(suffix one two three)
  add_executable(loop_${suffix} loop_${suffix}.cpp)
  target_compile_definitions(loop_${suffix} PRIVATE DEFINE_loop_${suffix})
endforeach()

macro(macro_target suffix)
  add_executable(macro_${suffix} macro_${suffix}.cpp)
  target_compile_definitions(macro_${suffix} PRIVATE DEFINE_macro_${suffix})
endmacro()

macro_target(one)
macro_target(two)
macro_target(three)

function(function_target suffix)
  add_executable(function_${suffix} function_${suffix}.cpp)
  target_compile_definitions(function_${suffix} PRIVATE DEFINE_function_${suffix})
endfunction()

function_target(one)
function_target(two)
function_target(three)

# TODO
# add_custom_target(check_metadata ALL
#   COMMAND ${CMAKE_COMMAND} -P check_metadata_files.cmake
# )
