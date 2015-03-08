
#ifndef STATIC_DEP_H
#define STATIC_DEP_H

#include <static_public.h>

#if defined(STATIC_PUBLIC) && defined(STATIC_PUBLIC_H)
int static_dep();
#endif

#endif
