cmake_minimum_required (VERSION 3.0)
project(SystemInformation)

include_directories("This does not exists")
get_directory_property(incl INCLUDE_DIRECTORIES)
set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${SystemInformation_BINARY_DIR};${SystemInformation_SOURCE_DIR}")

message("To prevent CTest from stripping output, you have to display: CTEST_FULL_OUTPUT")


configure_file(${SystemInformation_SOURCE_DIR}/SystemInformation.in
${SystemInformation_BINARY_DIR}/SystemInformation.out)
configure_file(${SystemInformation_SOURCE_DIR}/DumpInformation.h.in
${SystemInformation_BINARY_DIR}/DumpInformation.h)
add_executable(SystemInformation DumpInformation.cxx)

macro(FOO args)
  message("Test macro")
endmacro()

FOO(lala)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt "")
get_cmake_property(res VARIABLES)
foreach(var ${res})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt
             "${var} \"${${var}}\"\n")
endforeach()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/AllCommands.txt "")
get_cmake_property(res COMMANDS)
foreach(var ${res})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllCommands.txt
             "${var}\n")
endforeach()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/AllMacros.txt "")
get_cmake_property(res MACROS)
foreach(var ${res})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllMacros.txt
             "${var}\n")
endforeach()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/OtherProperties.txt "")
get_directory_property(res INCLUDE_DIRECTORIES)
foreach(var ${res})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/OtherProperties.txt
    "INCLUDE_DIRECTORY: ${var}\n")
endforeach()

get_directory_property(res LINK_DIRECTORIES)
foreach(var ${res})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/OtherProperties.txt
    "LINK_DIRECTORIES: ${var}\n")
endforeach()

get_directory_property(res INCLUDE_REGULAR_EXPRESSION)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/OtherProperties.txt
  "INCLUDE_REGULAR_EXPRESSION: ${res}\n")

macro(test_compiler_capability NAME CODE)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp" "${CODE}
int main() { return 0; }
")
  unset(${NAME} CACHE)
  try_compile(${NAME} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp"
    OUTPUT_VARIABLE OUTPUT
  )
  message("${NAME}: ${${NAME}}")
  if (NOT ${NAME})
    message("OUTPUT: ${OUTPUT}")
  endif()
endmacro()

test_compiler_capability(HAVE_MEMBER_TEMPLATES "
class Foo
{
  template<typename T>
  void foo(T t) { (void)t; }
};
")

test_compiler_capability(HAVE_VECTOR_AT "
#include <vector>
int foo()
{
  std::vector<int> vec;
  vec.push_back(1);
  return vec.at(0);
}
")

test_compiler_capability(HAVE_STRING_APPEND "
#include <string>
void foo()
{
  std::string s;
  s.append(\"content\");
}
")

test_compiler_capability(HAVE_STRING_CLEAR "
#include <string>
void foo()
{
  std::string s;
  s.clear();
}
")

test_compiler_capability(HAVE_BINARY_SEARCH_CONVERSION "
#include <vector>
#include <string>
#include <algorithm>
void foo()
{
  const char* needle = \"content\";
  std::vector<std::string> vec;
  std::binary_search(vec.begin(), vec.end(), needle);
}
")

test_compiler_capability(HAVE_TEMPLATE_NON_FUNCTION_PARAMETERS "
template<typename PropertyType>
PropertyType getTypedProperty(const char* prop)
{
  PropertyType t;
  return t;
}

int foo()
{
  return getTypedProperty<int>(0);
}
")


test_compiler_capability(HAVE_TEMPLATE_PARAMETER_DEFAULTED_POINTER "
template<typename PropertyType>
PropertyType getTypedProperty(const char* prop,
                              PropertyType* = 0)
{
  PropertyType t;
  return t;
}

int foo()
{
  return getTypedProperty<int>(0);
}
")

test_compiler_capability(HAVE_STD_SIZE_T "
#include <cstddef>

void foo()
{
  std::size_t sz = sizeof(int);
}
")

test_compiler_capability(HAVE_TEMPLATE_FRIENDS "
#include <cstddef>

template<typename T>
void bar();

class Foo
{
private:
  template<typename T> friend void bar();
  void foo() {}
};

template<typename T>
void bar()
{
  Foo f;
  f.foo();
}
")
