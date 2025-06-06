# Windows

Windows support is provided using the mingw enviroment.
It can be either cross-compiled from Linux or built using MSYS2.

# Cross Compiling

Cross compiling on Linux can be done using the `mingw-w64-toolchain`. Note this has been tested using the packages available on ArchLinux. Toolchains on other distros might give different results. However, binaries built this way on ArchLinux require Windows 7 or later.

# MSYS2

In MSYS2 there are multiple enviorements available. They differ in the used C and C++ Library. https://www.msys2.org/docs/environments/
Due this fact all modules must be built in the same enviorement. 
The build system creates custom tuples for them to keep them separated.

`UCRT64`, `CLANG64` and `CLANGARM64` use the `ucrt` C library, which is a modern C library.

`MINGW32` and `MINGW64` use the `msvcrt` C library, which is a C89 library. This limits the available langauge features, however, this allows a `MINGW32` build to run on a Windows version as low as Windows 2000.




