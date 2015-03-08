
#include <foo_linux.h>

#if (defined(__linux) || defined(__linux__) || defined(linux)) && defined(PLATFORM_IS_Linux)
int foo_linux()
{
  return 42;
}
#endif
