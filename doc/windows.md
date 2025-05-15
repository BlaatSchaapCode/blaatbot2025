#  Windows

Windows binaries can either be cross compiled on a Linux system or using  MSYS2 on Windows.

## Toolchain

Targetting Windows is currently done using mingw, using either the gcc or the clang compiler. Other compilers like Microsoft Visual Studio (Microsoft Optimizing C/C++ Compiler, or CL) are currently out of scope due to architectural differences.

Notable differences: The build system is made using GNU/Make. It is difficult to integrate CL as a compiler in such build system. It takes different flags, build dependencies are handled in a different way, etc.  To build using CL, a MSBuild projects needs to be made.

Different default linkage behaviour. On a gcc/clang build of a dynamic library, symbols are exported by default. On CL, this must be done manually. A common solution is to use `__declspec(dllexport)` cluttering the source code. An alternative would be a `.def` file given to the linker.

Furthermore, the way C and C++ libraries are handled is different. On a *NIX system, one links against the system's C and C++ libraries. In the Windows world, a toolchain brings its own libraries. For Visual Studio versions prior to 2015, each of them has their own versions. Visual Studio 2015 and later all use a compatible runtime.

As this code will need a modern C++ compiler, we are not bothered with these legacy versions of the libraries, but there is another thing to concern with runtime library handling in the Microsoft world. They use a different library depending on making a release or debug build.

There are things regarding passing objects between library borders that, from my understanding, is an issue limited to Windows, and possibly Microsoft Compilers only.

For the time being, Windows support is provided using the MINGW toolchain.

## Windows Version

Windows support is currently limited to Windows 10. When Thread Name support, a debugging feature is disabled, the resulting binary should be able to run on Windows 7. To support older Windows versions, some investigation is required. The target is to be able to run on Windows XP and ReactOS.

The solution to this issue would be to provide stubs for the missing API calls. This will probaly be a stub loader library in between the application and KERNEL32.DLL. For API calls directly made from the application, such as the thread name support, this can be trivially achieved. However, to do this for calls made from some library, such as a call to `GetDynamicTimeZoneInformation`, needs to be investigated.

## Windows Architectures

Windows builds can be made for the i686 and x86_64 architectures using the gcc compiler. 

The clang compiler can additionally target ARM, both 32 and 64 bit. It is not possible to cross compile to aarch64 on Linux due lack of libraries, but it is possible on MSYS2, though this has not been tested. Whether it is possible to create 32-bit ARM builds is at the time of writing not known to me, but such builds are a niche, as 32 bit ARM Windows versions are limited to Windows RT.

## Out of scope

The set goal is to support Windows XP/ReactOS. It is not intended to put effort to support even older Windows versions. Windows 2000 might work due its similarities to XP. However, supporting NT 4.0 or even NT 3.x ain't a goal. 

The Windows 9x systems (95,98,ME) are a completely different architecture, and this might make it difficult to support. 

Win32S, the subset of the WIN32 API that could be installed on top of Windows 3.x, as well as 16 bit Windows is out of scope, due the lack of threading support on these systems.


Windows on other Architectures, ALPHA, Itanium, MIPS, PowerPC, are historical. No modern toolchain supports targetting these architectures.

| Architecture | Latest Version   |
| ------------ | --------------   |
| Alpha        | Windows 2000 RC2 |
| Itanium      | Windows XP       |
| MIPS         | NT 4.0           |
| PowerPC      | NT 4.0           |

While Itanium is supported on Windows XP, and XP is a supported goal, supporting Itanium builds will not be considered. The hardware is rare and there are currently no emulators evailable for this architecture. 


## Setting up on ArchLinux

## Setting up on MSYS2