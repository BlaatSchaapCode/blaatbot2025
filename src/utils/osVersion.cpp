/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include "osVersion.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

//#include <dbghelp.h>

#include <string>

void detectWindowsVersion(void) {
    OSVERSIONINFOA osversioninfoa = {0};
    osversioninfoa.dwOSVersionInfoSize = sizeof(osversioninfoa);
    GetVersionExA(&osversioninfoa);

    printf("GetVersionExW Reported Windows version %d.%d.%d platform id %d\n", osversioninfoa.dwMajorVersion,
           osversioninfoa.dwMinorVersion, osversioninfoa.dwBuildNumber, osversioninfoa.dwPlatformId);

    //    OSVERSIONINFOEXA osversioninfoexa = {0};
    //    osversioninfoexa.dwOSVersionInfoSize = sizeof(osversioninfoexa);
    //    GetVersionExA((LPOSVERSIONINFOW)&osversioninfoexa);

    // ReactOS detect
    // https://reactos.org/forum/viewtopic.php?t=1401
    char reactos[128] = {};
    strncpy(reactos, osversioninfoa.szCSDVersion + strlen(osversioninfoa.szCSDVersion) + 1,
            sizeof(osversioninfoa.szCSDVersion) - (strlen(osversioninfoa.szCSDVersion) + 1));

    if (strlen(reactos)) {
        printf("Running on %s\n", reactos);
    } else {
        printf("Not running on ReactOS\n");
    }

    // If the application has no manifest this will return NT 6.2 (Windows 8)
    // on any later releases.
    // MSYS64 builds automatically include a manifest
    // Cross compiled builds do not

    // Alternatively, we can query the kernel directly. However, this only
    // works if our application is built for the same architecture as the
    // kernel. eg. an x86_64 (AMD64) binary will run on an aarch64 (ARM64)
    // kernel, but not when we link in the kernel (NtosKrnl.exe)

    // Note: Is the kernel always called "NtosKrnl.exe" or do we have to
    // obtain the current kernel's filename somehow?

    RTL_OSVERSIONINFOW rtl_osversioninfow = {0};
    rtl_osversioninfow.dwOSVersionInfoSize = sizeof(rtl_osversioninfow);

    auto kernel_library_handle = LoadLibrary("NtosKrnl.exe");
    if (kernel_library_handle) {
        typedef NTSTATUS (*RtlGetVersion_f)(PRTL_OSVERSIONINFOW lpVersionInformation);
        RtlGetVersion_f rtlGetVersion = (RtlGetVersion_f)GetProcAddress(kernel_library_handle, "RtlGetVersion");

        if (rtlGetVersion) {
            rtlGetVersion(&rtl_osversioninfow);

            printf("RtlGetVersion Reported Windows version %d.%d.%d platform id %d\n", rtl_osversioninfow.dwMajorVersion,
                   rtl_osversioninfow.dwMinorVersion, rtl_osversioninfow.dwBuildNumber, rtl_osversioninfow.dwPlatformId);

        } else {
            // We opened the kernel as a library, but we were unable to obtain
            // the symbol. RtlGetVersion was added in Windows 2000 (NT 5.0)
            // Are we running on NT 4 or NT 3 ?
            // (We are on NT because we were able to load the NT kernel_library_handle)
            printf("RtlGetVersion not found in kernel\n");
        }
    } else {
        // Unable to open kernel as a library,
        // might be running a different architecture?
        printf("Unable to load kernel as a library\n");
    }

    // Let's have a look at determining the architecture.

    SYSTEM_INFO system_info = {0};
    GetSystemInfo(&system_info);
    SYSTEM_INFO native_system_info = {0};
    GetNativeSystemInfo(&native_system_info);
    // When running an x86_64 (AMD64) binary on an aarch64 (ARM64) kernel
    // calling the API to get the architecture lies to us.
    printf("GetSystemInfo report ProcessorArchitecture as       %x\n", system_info.wProcessorArchitecture);
    printf("GetNativeSystemInfo report ProcessorArchitecture as %x\n", native_system_info.wProcessorArchitecture);
    // Might want to test IsWow64Process2 too?

    // Alternatively, when running on NT, we can look at the kernel's binary
    // This should give away the architecture of the kernel we are running

    IMAGE_NT_HEADERS peHdr = {};
    wchar_t windowsDirectory[PATH_MAX + 1] = {};
    // GetWindowsDirectoryW(windowsDirectory, PATH_MAX);
    GetSystemWindowsDirectoryW(windowsDirectory, PATH_MAX);
    // Is the NT Kernel **always** NtosKrnl.exe or do we need to query the file name?
    std::wstring windowsKernelPath = std::wstring(windowsDirectory) + L"\\system32\\NtosKrnl.exe";

    printf("Windows directory is %ls\n", windowsDirectory);
    printf("Kernel is %ls\n", windowsKernelPath.c_str());

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
                    printf("Machine is %x\n", peHdr.FileHeader.Machine);
                } else {
                    printf("Unable to extract information from kernel\n");
                }
            }
            CloseHandle(hMapping);
        } else {
            printf("Unable to map kernel\n");
        }
        CloseHandle(hFile);
    } else {
        printf("Unable to open kernel as a file\n");
    }

    // Another source we could query for version information is the
    // registry. However, these values can be edited by the user.
    // https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-reggetvaluea
    DWORD pcbData = 0;

    // Windows XP:
    // SZ CurrentVersion
    // SZ CurrentBuildNumber
    // SZ CSDVersion
    // SZ ProductName


    // Windows 10:
    // DWORD CurrentMajorVersionNumber;
    // DWORD CurrentMinorVersionNumber;
    // SZ CurrentBuildNumber
    // SZ CSDVersion
    // SZ ProductName


    // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
    // ERROR_FILE_NOT_FOUND
    DWORD CurrentMajorVersionNumber = 0;
    pcbData = sizeof(CurrentMajorVersionNumber);
    LSTATUS status = RegGetValueA(HKEY_LOCAL_MACHINE,                             // hkey
                                  "SOFTWARE\Microsoft\Windows NT\CurrentVersion", // lpSubKey
                                  "CurrentMajorVersionNumber",                    // lpValue
                                  RRF_RT_DWORD | RRF_ZEROONFAILURE,               // dwFlags
                                  nullptr,                                        // pdwType
                                  &CurrentMajorVersionNumber,                     // pvData
                                  &pcbData                                        // pcbData
    );
    printf("Reg CurrentMajorVersionNumber status %x Value %d\n", status, CurrentMajorVersionNumber);

    char CurrentVersion[32] = {};
    pcbData = sizeof(CurrentVersion);
    status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
                          "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                          "CurrentVersion",                                  // lpValue
                          RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
                          nullptr,                                           // pdwType
                          &CurrentVersion,                                   // pvData
                          &pcbData                                           // pcbData
    );
    printf("Reg CurrentVersion status %x Value %s\n", status, CurrentVersion);

    // Checking for wine
    // https://gist.github.com/klange/282e62fdd1c2cae6a210443ccbc5b12f
    // https://gist.github.com/ork/32da69687c94530931ed

    typedef const char *(*wine_get_version_f)(void);
    typedef const char *(*wine_get_build_id_f)(void);
    typedef const char *(*wine_get_host_version_f)(const char **sysname, const char **release);

    wine_get_version_f wine_get_version = nullptr;
    wine_get_host_version_f wine_get_host_version = nullptr;
    wine_get_build_id_f wine_get_build_id = nullptr;

    HMODULE hntdll = GetModuleHandle("ntdll.dll");
    if (hntdll) {
        wine_get_version = (wine_get_version_f)GetProcAddress(hntdll, "wine_get_version");
        wine_get_host_version = (wine_get_host_version_f)GetProcAddress(hntdll, "wine_get_host_version");
        // wine_get_build_id = (wine_get_build_id_f)GetProcAddress(hntdll, "wine_get_build_id_f");
        //  wine_get_build_id

        if (wine_get_version && wine_get_host_version) {
            const char *sysname;
            const char *version;
            wine_get_host_version(&sysname, &version);
            fprintf(stdout, "Running Wine %s under %s %s.\n", wine_get_version(), sysname, version);
            // Outputs "Running Wine 10.8 under Linux 6.14.6-arch1-1."
            // fprintf(stdout, "Build ID  %s.\n", wine_get_build_id());
        } else {
            fprintf(stdout, "Not running under wine.\n");
        }
    }
}

#endif
