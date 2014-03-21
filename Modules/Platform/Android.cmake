include(Platform/Linux)

# Android has soname, but binary names must end in ".so" so we cannot append
# a version number.  Also we cannot portably represent symlinks on the host.
set(CMAKE_PLATFORM_NO_VERSIONED_SONAME 1)

# Android reportedly ignores RPATH, and we cannot predict the install
# location anyway.
set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "")

# Nsight Tegra Visual Studio Edition takes care of
# prefixing library names with '-l'.
if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
  set(CMAKE_LINK_LIBRARY_FLAG "")
endif()

# Gui executables on Android are loaded as plugins by the java process,
# so build them as PIC by default.  The packaging system accepts only files
# which have a .so suffix, so set the CMAKE_EXECUTABLE_SUFFIX to that.
# Non-gui native executables are also permitted on Android.  Users wishing
# to create such executables may use something like
#   set_property(TARGET MyConsoleExe PROPERTY SUFFIX "")
#   set_property(TARGET MyConsoleExe PROPERTY POSITION_INDEPENDENT_CODE OFF)
# to clear the suffix and -fPIE flag, if desired.
# This possibly does not work with older Android versions:
#  https://groups.google.com/forum/#!msg/android-security-discuss/B9BEdc_faRw/iMjpQqXMA1YJ
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXECUTABLE_SUFFIX .so)
