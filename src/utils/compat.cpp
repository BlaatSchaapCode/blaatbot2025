
#if defined __clang__ && defined _WIN32
/*
    This fixes a linker error when (cross-)compiling for MINGW using clang
    /usr/bin/x86_64-w64-mingw32-ld:
   bld/clang/mingw/x86_64/debug/src/main.cpp.o:main.cpp:(.rdata$.refptr.__libc_single_threaded[.refptr.__libc_single_threaded]+0x0):
   undefined reference to `__libc_single_threaded'

    /usr/bin/x86_64-w64-mingw32-ld:
   bld/clang/mingw/x86_64/debug/src/utils/compat.cpp.o:compat.cpp:(.debug_info+0x2b):
   undefined reference to `__libc_single_threaded'
   /usr/bin/x86_64-w64-mingw32-ld:
   bld/clang/mingw/x86_64/debug/src/main.cpp.o:main.cpp:(.rdata$.refptr.__libc_single_threaded[.refptr.__libc_single_threaded]+0x0):
   undefined reference to `__libc_single_threaded' clang-13: error: linker
   command failed with exit code 1 (use -v to see invocation)


    ld.lld: error: undefined symbol: __libc_single_threaded
    >>> referenced by
   bld/clang/mingw/x86_64/debug/src/main.cpp.o:(.refptr.__libc_single_threaded)
*/
#include <cstdbool>
extern "C" {
bool __libc_single_threaded = false;
}
#endif

//
// #if defined(_WIN32) || defined(_WIN64)
// #include <windows.h>
// #include <logger.hpp>
//// When we do this in the file where it is used, we override the implementation
//// When we try to do this in another source file, we get a multiple definition linker error.
//// This means, we'll need to do something in a linker file making it drop the definition
//// from the library so we only have our override. Or are there other options?

//
// HRESULT SetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription) {
//    // This triggers a
//    // "redeclared without dllimport attribute: previous dllimport ignored [-Wattributes]"
//    // So, we are actually overriding what was in there. This means the executable should
//    // be able to run on older windows versions. Then if we open the dll here and try
//    // to get the function pointer, if it succeeds, we call it, otherwise we do nothing.
//    // This is a debug feature after all, nothing essential for it to run.
//    static bool initialised = false;
//    typedef HRESULT (*SetThreadDescription_f)(HANDLE hThread, PCWSTR lpThreadDescription);
//    static SetThreadDescription_f f = nullptr;
//    static HMODULE handle = nullptr;
//    if (!initialised) {
//        initialised = true;
//        handle = LoadLibrary("Kernel32.dll");
//        if (handle) {
//            f = (SetThreadDescription_f)GetProcAddress(handle, "SetThreadDescription");
//        }
//        if (f) {
//            LOG_INFO("SetThreadDescription is supported on this kernel");
//        } else {
//            LOG_INFO("SetThreadDescription is supported not on this kernel");
//        }
//    }
//
//    if (f)
//        return f(hThread, lpThreadDescription);
//    return E_NOTIMPL;
//}
// #endif
