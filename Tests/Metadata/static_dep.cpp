
#include "static_dep.h"

#include <static_private.h>

#if defined(STATIC_PUBLIC) && defined(STATIC_PUBLIC_H)
int static_dep()
{
  return 42;
}
#endif

int static_user()
{
  return static_dep();
}
  
