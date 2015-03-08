
#include <foo_release.h>

#ifdef CONFIG_IS_RELEASE
int foo_release()
{
  return 42;
}
#endif
