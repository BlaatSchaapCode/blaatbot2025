#include "osVersion.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <dbghelp.h>
#include <windows.h>

#include <string>

void detectWindowsVersion(void) {
    OSVERSIONINFOW osversioninfow = {0};
    osversioninfow.dwOSVersionInfoSize = sizeof(osversioninfow);
    GetVersionExW(&osversioninfow);

    OSVERSIONINFOEXW osversioninfoexw = {0};
    osversioninfoexw.dwOSVersionInfoSize = sizeof(osversioninfoexw);
    GetVersionExW((LPOSVERSIONINFOW)&osversioninfoexw);

    // If the application has no manifest this will return NT 6.2 (Windows 8)
    // on any later releases.
    // Hmmm... what is going on? I am getting the **correct** version
    // on a MINGW64 UCRT64 build. Does it auto-matifest? as I haven't
    // included any manifest yet. Well.. that makes life easier, doesn't it?

    // Alternatively, we can query the kernel directly. However, this only
    // works if our application is built for the same architecture as the
    // kernel. eg. an x86_64 (AMD64) binary will run on an aarch64 (ARM64)
    // kernel, but not when we link in the kernel (NtosKrnl.exe)

    // Note: Is the kernel always called "NtosKrnl.exe" or do we have to
    // obtain the current kernel's filename somehow?

    RTL_OSVERSIONINFOW rtl_osversioninfow = {0};
    rtl_osversioninfow.dwOSVersionInfoSize = sizeof(rtl_osversioninfow);

    RTL_OSVERSIONINFOEXW rtl_osversioninfoexw = {0};
    rtl_osversioninfoexw.dwOSVersionInfoSize = sizeof(rtl_osversioninfoexw);

    auto kernel_library_handle = LoadLibrary("NtosKrnl.exe");
    if (kernel_library_handle) {
        typedef NTSTATUS (*RtlGetVersion_f)(PRTL_OSVERSIONINFOW lpVersionInformation);
        RtlGetVersion_f rtlGetVersion = (RtlGetVersion_f)GetProcAddress(kernel_library_handle, "RtlGetVersion");

        typedef NTSTATUS (*RtlGetVersionEx_f)(PRTL_OSVERSIONINFOEXW lpVersionInformation);
        RtlGetVersionEx_f rtlGetVersionEx = (RtlGetVersionEx_f)GetProcAddress(kernel_library_handle, "RtlGetVersion");

        if (rtlGetVersion) {
            rtlGetVersion(&rtl_osversioninfow);
            rtlGetVersionEx(&rtl_osversioninfoexw);
        } else {
            // We opened the kernel as a library, but we were unable to obtain
            // the symbol. RtlGetVersion was added in Windows 2000 (NT 5.0)
            // Are we running on NT 4 or NT 3 ?
            // (We are on NT because we were able to load the NT kernel_library_handle)
        }
    } else {
        // Unable to open kernel as a library,
        // might be running a different architecture?
    }

    // Let's have a look at determining the architecture.

    SYSTEM_INFO system_info = {0};
    GetSystemInfo(&system_info);
    SYSTEM_INFO native_system_info = {0};
    GetNativeSystemInfo(&native_system_info);
    // When running an x86_64 (AMD64) binary on an aarch64 (ARM64) kernel
    // calling the API to get the architecture lies to us.
    // Might want to test IsWow64Process2 too?

    // Alternatively, when running on NT, we can look at the kernel's binary
    // This should give away the architecture of the kernel we are running

    IMAGE_NT_HEADERS peHdr = {};
    wchar_t windowsDirectory[PATH_MAX + 1] = {};
    GetWindowsDirectoryW(windowsDirectory, PATH_MAX);
    // Is the NT Kernel **always** NtosKrnl.exe or do we need to query the file name?
    std::wstring windowsKernelPath = std::wstring(windowsDirectory) + L"\\NtosKrnl.exe";

    HANDLE hFile =
        CreateFileW(windowsKernelPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, nullptr);
        if (hMapping != INVALID_HANDLE_VALUE) {
            LPVOID addrHeader = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
            if (addrHeader) {
                PIMAGE_NT_HEADERS ppeHdr = ImageNtHeader(addrHeader);
                if (ppeHdr) {
                    peHdr = *ppeHdr;
                }
            }
            CloseHandle(hMapping);
        }
        CloseHandle(hFile);
    }
}

#endif
