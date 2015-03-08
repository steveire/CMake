
#include <foo_msvc_gt_1800_debug.h>

#if defined(_MSC_VER) && _MSC_VER > 1800 && defined(CONFIG_IS_DEBUG)
int foo_msvc_gt_1800_debug()
{
  return 42;
}
#endif
