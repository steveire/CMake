
#include <foo_gcc_lt_44.h>

#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 404
int foo_gcc_lt_44()
{
  return 42;
}
#endif
