
#include <foo_debug.h>

#ifdef CONFIG_IS_DEBUG
int foo_debug()
{
  return 42;
}
#endif
