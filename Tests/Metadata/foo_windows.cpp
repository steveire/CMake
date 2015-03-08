
#include <foo_windows.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
int foo_windows()
{
  return 42;
}
#endif
