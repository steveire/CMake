
#include <shared_dep.h>
#include <static_dep.h>
#include <obj_dep.h>
#include <bar_file.h>
#include <c_file.h>

#ifdef CONFIG_IS_DEBUG
#include <foo_debug.h>
#endif
#ifdef CONFIG_IS_RELEASE
#include <foo_release.h>
#endif
#if CONFIG_IS_NOT_DEBUG
#include <foo_not_debug.h>
#endif
#if CONFIG_IS_NOT_RELEASE
#include <foo_not_release.h>
#endif
#if (defined(__linux) || defined(__linux__) || defined(linux)) && defined(PLATFORM_IS_Linux)
#include <foo_linux.h>
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <foo_windows.h>
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) \
  || defined(__linux) || defined(__linux__) || defined(linux)
#include <foo_windows_or_linux.h>
#endif
#if ((defined(__linux) || defined(__linux__) || defined(linux)) && defined(__GNUC__)) && defined(CXX_COMPILER_IS_GNU)
#include <foo_linux_gcc.h>
#endif
#if (defined(__linux) || defined(__linux__) || defined(linux)) && defined(__clang__)
#include <foo_linux_clang.h>
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) > 404
#include <foo_gcc_gt_44.h>
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 404
#include <foo_gcc_lt_44.h>
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) == 404
#include <foo_gcc_eq_44.h>
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && CXX_COMPILER_HAS_VARIADIC_TEMPLATES
#include <foo_gcc_gte_44.h>
#endif
#if defined(_MSC_VER) && _MSC_VER == 1800 && defined(CONFIG_IS_DEBUG)
#include <foo_msvc_eq_1800_debug.h>
#endif
#if defined(_MSC_VER) && _MSC_VER > 1800 && defined(CONFIG_IS_DEBUG)
#include <foo_msvc_gt_1800_debug.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1800 && defined(CONFIG_IS_DEBUG)
#include <foo_msvc_gte_1800_debug.h>
#endif
#if defined(_MSC_VER) && _MSC_VER < 1800 && defined(CONFIG_IS_DEBUG)
#include <foo_msvc_lt_1800_debug.h>
#endif


extern int compiled_as_cxx();
extern int static_use_symbol();

#ifdef THIS_IS_THE_MAIN_FILE
int main()
{
  return shared_dep() - static_dep()
        + obj_dep() - compiled_as_cxx()
        + static_use_symbol() - bar_file()
        + c_file() - 42
#ifdef CONFIG_IS_DEBUG
        + foo_debug() - 42
#endif
#ifdef CONFIG_IS_RELEASE
        + foo_release() - 42
#endif
#if CONFIG_IS_NOT_DEBUG
        + foo_not_debug() - 42
#endif
#if CONFIG_IS_NOT_RELEASE
        + foo_not_release() - 42
#endif
#if (defined(__linux) || defined(__linux__) || defined(linux)) && defined(PLATFORM_IS_Linux)
        + foo_linux() - 42
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        + foo_windows() - 42
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) \
  || defined(__linux) || defined(__linux__) || defined(linux)
        + foo_windows_or_linux() - 42
#endif
#if ((defined(__linux) || defined(__linux__) || defined(linux)) && defined(__GNUC__)) && defined(CXX_COMPILER_IS_GNU)
        + foo_linux_gcc() -42
#endif
#if (defined(__linux) || defined(__linux__) || defined(linux)) && (defined(__clang__))
        + foo_linux_clang() -42
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) > 404
        + foo_gcc_gt_44() - 42
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 404
        + foo_gcc_lt_44() - 42
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) == 404
        + foo_gcc_eq_44() - 42
#endif
#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && CXX_COMPILER_HAS_VARIADIC_TEMPLATES
        + foo_gcc_gte_44() - 42
#endif
#if defined(_MSC_VER) && _MSC_VER == 1800 && defined(CONFIG_IS_DEBUG)
        + foo_msvc_eq_1800_debug() - 42
#endif
#if defined(_MSC_VER) && _MSC_VER > 1800 && defined(CONFIG_IS_DEBUG)
        + foo_msvc_gt_1800_debug() - 42
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1800 && defined(CONFIG_IS_DEBUG)
        + foo_msvc_gte_1800_debug() - 42
#endif
#if defined(_MSC_VER) && _MSC_VER < 1800 && defined(CONFIG_IS_DEBUG)
        + foo_msvc_lt_1800_debug() - 42
#endif
    ;
    // AUTOCOMPLETE_CURSOR_POSITION
}
#endif
