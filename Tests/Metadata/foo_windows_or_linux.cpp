
#include "foo_windows_or_linux.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) \
  || defined(__linux) || defined(__linux__) || defined(linux)
int foo_windows_or_linux()
{
  return 42;
}
#endif
