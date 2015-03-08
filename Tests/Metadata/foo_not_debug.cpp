
#include <foo_not_debug.h>

#if CONFIG_IS_NOT_DEBUG
int foo_not_debug()
{
  return 42;
}
#endif
