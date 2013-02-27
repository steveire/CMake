
#include "common.h"
#include "publicinclude.h"
#include "interfaceinclude.h"
#include "relative_dir.h"
#include "consumer.h"
#include "list1.h"
#include "list2.h"
#include "list3.h"
#include "list4.h"
#ifdef TEST_LANG_DEFINES
  #include "cxx_only.h"
#endif

#ifdef PRIVATEINCLUDE_DEFINE
#error Unexpected PRIVATEINCLUDE_DEFINE
#endif

#ifndef PUBLICINCLUDE_DEFINE
#error Expected PUBLICINCLUDE_DEFINE
#endif

#ifndef INTERFACEINCLUDE_DEFINE
#error Expected INTERFACEINCLUDE_DEFINE
#endif

#ifndef CURE_DEFINE
#error Expected CURE_DEFINE
#endif

#ifndef RELATIVE_DIR_DEFINE
#error Expected RELATIVE_DIR_DEFINE
#endif

#ifndef CONSUMER_DEFINE
#error Expected CONSUMER_DEFINE
#endif

#ifdef TEST_LANG_DEFINES
  #ifndef CXX_ONLY_DEFINE
  #error Expected CXX_ONLY_DEFINE
  #endif
#endif

#ifndef LIST1_DEFINE
#error Expected LIST1_DEFINE
#endif

#ifndef LIST2_DEFINE
#error Expected LIST2_DEFINE
#endif

#ifndef LIST3_DEFINE
#error Expected LIST3_DEFINE
#endif

#ifndef LIST4_DEFINE
#error Expected LIST4_DEFINE
#endif


int main() { return 0; }
