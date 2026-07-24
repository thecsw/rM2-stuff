# CMake toolchain file for cross-compiling rM2-stuff apps for the
# reMarkable 2 (armv7-a hard-float, glibc / "arm-linux-gnueabihf").
#
# Usage:
#   cmake --preset cross-armhf      (see CMakePresets.json)
#
# The resulting binary is a normal dynamically-linked ELF that runs on the
# device's Codex Linux (glibc). It only needs libudev.so.1 / libstdc++ / libc
# at runtime, all of which are present on the reMarkable.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple arm-linux-gnueabihf)

set(CMAKE_C_COMPILER "${triple}-gcc")
set(CMAKE_CXX_COMPILER "${triple}-g++")
set(CMAKE_AR "${triple}-ar" CACHE FILEPATH "")
set(CMAKE_RANLIB "${triple}-ranlib" CACHE FILEPATH "")
set(CMAKE_STRIP "${triple}-strip" CACHE FILEPATH "")

# Tuning: armv7-a + NEON + hard-float, matches the reMarkable 2 CPU (Cortex-A7).
set(CMAKE_C_FLAGS_INIT   "-march=armv7-a -mfpu=neon -mfloat-abi=hard -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_INIT "-march=armv7-a -mfpu=neon -mfloat-abi=hard -fomit-frame-pointer")

# Prefer the cross sysroot packages (libevdev/libudev) but fall back to the
# default system search for libc/libstdc++.
set(CMAKE_FIND_ROOT_PATH "/usr/${triple}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# pkg-config for the cross arch.
set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/${triple}/pkgconfig:/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "/")
