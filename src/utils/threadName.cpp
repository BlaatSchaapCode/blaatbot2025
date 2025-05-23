#include "threadName.hpp"
#include "logger.hpp"

#if defined(_WIN32) || defined(_WIN64)
// setThreadName for Windows
// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
// Documentation states minimal version is Windows 10, version 1607
// Thus it might need some work to make something that can at least run as no-op
// on older versions

#include <windows.h>
#define WSTRING_SIZE (16)

// When we do this in the file where it is used, we override the implementation
// When we try to do this in another source file, we get a multiple definition linker error

HRESULT SetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription) {
    // This triggers a
    // "redeclared without dllimport attribute: previous dllimport ignored [-Wattributes]"
    // So, we are actually overriding what was in there. This means the executable should
    // be able to run on older windows versions. Then if we open the dll here and try
    // to get the function pointer, if it succeeds, we call it, otherwise we do nothing.
    // This is a debug feature after all, nothing essential for it to run.
    static bool initialised = false;
    typedef HRESULT (*SetThreadDescription_f)(HANDLE hThread, PCWSTR lpThreadDescription);
    static SetThreadDescription_f f = nullptr;
    static HMODULE handle = nullptr;
    if (!initialised) {
        initialised = true;
        handle = LoadLibrary("Kernel32.dll");
        if (handle) {
            f = (SetThreadDescription_f)GetProcAddress(handle, "SetThreadDescription");
        }
        if (f) {
            LOG_INFO("SetThreadDescription supported on this kernel");
        } else {
            LOG_INFO("SetThreadDescription supported not on this kernel");
        }
    }

    if (f)
        return f(hThread, lpThreadDescription);
    return E_NOTIMPL;
}

void setThreadName(std::string threadName) {
    wchar_t buff[WSTRING_SIZE];
    auto result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, threadName.c_str(), -1, buff, WSTRING_SIZE);
    if (result > 0)
        SetThreadDescription(GetCurrentThread(), buff);
}
#elif (__GNUC__ && __linux__) || __FreeBSD__
// setThreadName for the GNU C Library
#include <pthread.h>

#ifdef __FreeBSD__
#include <pthread_np.h>
#endif

void setThreadName(std::string threadName) {
    /*
       The thread name is a
       meaningful C language string, whose length is restricted to 16
       characters, including the terminating null byte ('\0').
     */

    int result = pthread_setname_np(pthread_self(), threadName.c_str());
    (void)(result);
}
#elif defined __BEOS__ || defined __HAIKU__
// Set threadName under Haiku
#include <kernel/OS.h>

void setThreadName(std::string threadName) { rename_thread(find_thread(nullptr), threadName.c_str()); }

#else
// Empty implementation for non-supported OS'es/ C Libraries
// TODO: add others, see
// https://stackoverflow.com/questions/2369738/how-to-set-the-name-of-a-thread-in-linux-pthreads
// for NetBSD, FreeBSD, OpenBSD
void setThreadName(std::string threadName) {}
#endif
