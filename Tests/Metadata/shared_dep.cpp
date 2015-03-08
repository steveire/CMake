
#include "shared_dep.h"

#include <shared_private.h>

#if defined(SHARED_PUBLIC) && defined(SHARED_PUBLIC_H)
int shared_dep()
{
  return 42;
}
#endif

int shared_user()
{
  return shared_dep();
}
