# - Function for generation of export macros for libraries
# This module provides the function GENERATE_EXPORT_HEADER() and the
# accompanying ADD_COMPILER_EXPORT_FLAGS() function.
#
# - Check whether the CXX linker supports a given flag.
# CHECK_CXX_LINKER_FLAG(<flag> <var>)
#  <flag> - the linker flag
#  <var>  - variable to store the result
# This internally calls the check_cxx_source_compiles macro.  See help
# for CheckCXXSourceCompiles for a listing of variables that can
# modify the build.

#=============================================================================
# Copyright 2011 Stephen Kelly <steveire@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

include(CheckCXXCompilerFlag)

macro (CHECK_CXX_LINKER_FLAG _FLAG _RESULT)
   set(CMAKE_TRY_COMPILE_TYPE SHARED_LIBRARY)
   set(CMAKE_TRY_COMPILE_SHARED_LINKER_FLAGS ${_FLAG})
   check_cxx_source_compiles("int main() { return 0;}" ${_RESULT})
endmacro ()
