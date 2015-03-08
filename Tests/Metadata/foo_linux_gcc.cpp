
#include <foo_linux_gcc.h>

#if ((defined(__linux) || defined(__linux__) || defined(linux)) && defined(__GNUC__)) && defined(CXX_COMPILER_IS_GNU)
int foo_linux_gcc()
{
  return 42;
}
#endif
