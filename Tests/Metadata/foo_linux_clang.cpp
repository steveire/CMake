
#include <foo_linux_clang.h>

#if (defined(__linux) || defined(__linux__) || defined(linux)) && (defined(__clang__))
int foo_linux_clang()
{
  return 42;
}
#endif
