
set(CMAKE_SYSTEM_NAME Linux)

#set(CMAKE_SYSTEM_PROCESSOR MIPS)

set(ANDROID_NDK_PATH "/home/stephen/android/android-ndk-r8e")
set(CMAKE_SYSROOT "${ANDROID_NDK_PATH}/platforms/android-14/arch-mips")

set(ANDROID_TOOLS "${ANDROID_NDK_PATH}/toolchains/mipsel-linux-android-4.6/prebuilt/linux-x86_64/bin")

set(CMAKE_C_COMPILER "${ANDROID_TOOLS}/mipsel-linux-android-gcc" )
set(CMAKE_CXX_COMPILER "${ANDROID_TOOLS}/mipsel-linux-android-g++" )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
