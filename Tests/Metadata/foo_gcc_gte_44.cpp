
#include <foo_gcc_gte_44.h>

#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && CXX_COMPILER_HAS_VARIADIC_TEMPLATES
int foo_gcc_gte_44()
{
  return 42;
}
#endif
