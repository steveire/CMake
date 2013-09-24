
#include "testlib_export.h"

TESTLIB_DEPRECATED_EXPORT void freeDeprecatedMethod1(void) {

}
TESTLIB_DEPRECATED_EXPORT void freeDeprecatedMethod2(void) {

}

TESTLIB_DEPRECATED_EXPORT void anotherFreeDeprecatedMethod(void) {
  // Clang does not warn when a deprecated method calls another
  // deprecated method. GCC does.
  freeDeprecatedMethod1();
}

TESTLIB_EXPORT void freeNotDeprecatedMethod(void) {
  freeDeprecatedMethod2();
}

struct TESTLIB_DEPRECATED_EXPORT Deprecated
{
  TESTLIB_DEPRECATED void deprecatedMethod() {}

  void notDeprecatedMethod() {}
};

struct TESTLIB_EXPORT NotDeprecated
{
  TESTLIB_DEPRECATED void deprecatedMethod() {}

  void notDeprecatedMethod() {}
};

// Clang warns on deriving from a deprecated class. GCC does not.
struct TESTLIB_EXPORT Derived : public Deprecated
{
};

struct TESTLIB_DEPRECATED_EXPORT AnotherDeprecated
{
  void notDeprecatedMethod() {
    // Clang does not warn when a deprecated class is instantiated in a
    // method of a deprecated class. GCC does.
    Deprecated d1;
    d1.notDeprecatedMethod();
    // GCC warns when calling a deprecated method, even if
    // the caller is a method on a deprecated class.
    d1.deprecatedMethod();

    NotDeprecated nd1;
    nd1.notDeprecatedMethod();
    // GCC warns when calling a deprecated method on a not-deprecated
    // class, even if the caller is a method on a deprecated class.
    nd1.deprecatedMethod();
  }
};

struct TESTLIB_EXPORT AnotherNotDeprecated
{
  TESTLIB_DEPRECATED void deprecatedMethod() {

    // GCC warns about instantiating a deprecated class, even in a
    // deprecated method.
    Deprecated d2;
    d2.notDeprecatedMethod();
    // Clang does not warn when a deprecated class is instantiated in a
    // deprecated method of a not-deprecated class. GCC does.
    d2.deprecatedMethod();

    NotDeprecated nd2;
    nd2.notDeprecatedMethod();
    nd2.deprecatedMethod();
  }

  void notDeprecatedMethod() {

    Deprecated d3;
    d3.notDeprecatedMethod();
    // Clang and GCC both warn when calling a deprecated method, if the
    // caller is not deprecated
    d3.deprecatedMethod();

    NotDeprecated nd3;
    nd3.notDeprecatedMethod();
    // Clang and GCC both warn when calling a deprecated method, if the
    // caller is not deprecated
    nd3.deprecatedMethod();
  }
};

// Clang and GCC both warn about typedef to deprecated.
typedef Deprecated TypeDef;

// Clang and GCC both warn about deprecated type in templates.
template<typename T = Deprecated>
struct Template
{
};
