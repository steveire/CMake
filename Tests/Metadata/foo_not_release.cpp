
#include <foo_not_release.h>

#if CONFIG_IS_NOT_RELEASE
int foo_not_release()
{
  return 42;
}
#endif
