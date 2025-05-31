#include "osVersion.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#include <dbghelp.h>

#include <cstdint>
#include <string>

int getRegistryDword(HKEY hKey, const char *subkey, const char *value_name, int32_t *value) {
    // While the most convenient way to get a registry value is RegGetValueA,
    // this function was added in NT 5.2 (Win2003, XP64), we'll settle with a
    // sequence of calls to RegOpenKeyExA RegQueryValueExA RegCloseKey
    int status;
    HKEY hkResult;
    DWORD pcbData = sizeof(int32_t);
    status = RegOpenKeyExA(hKey, subkey, 0, KEY_READ, &hkResult);
    if (status)
        return status;
    status = RegQueryValueExA(hkResult, value_name, nullptr, nullptr, (LPBYTE)value, &pcbData);
    RegCloseKey(hkResult);
    return status;
}

int getRegistryString(HKEY hKey, const char *subkey, const char *value_name, char *value, size_t len) {
    // While the most convenient way to get a registry value is RegGetValueA,
    // this function was added in NT 5.2 (Win2003, XP64), we'll settle with a
    // sequence of calls to RegOpenKeyExA RegQueryValueExA RegCloseKey
    int status;
    HKEY hkResult;
    DWORD pcbData = len - 1;
    status = RegOpenKeyExA(hKey, subkey, 0, KEY_READ, &hkResult);
    if (status)
        return status;
    status = RegQueryValueExA(hkResult, value_name, nullptr, nullptr, (LPBYTE)value, &pcbData);
    value[len - 1] = 0; // ensure null termination
    RegCloseKey(hkResult);
    return status;
}

void detectWindowsVersion(void) {
    // WIN32 API programming is screaming at you with all those ALL-CAPS datatypes.

    // Let's start with basic version information.

    // We could pass either OSVERSIONINFOA, or OSVERSIONINFOEXA
    // to GetVersionExA. It determines with is used based on the
    // dwOSVersionInfoSize value.
    OSVERSIONINFOA osversioninfoa = {0};
    osversioninfoa.dwOSVersionInfoSize = sizeof(osversioninfoa);
    GetVersionExA(&osversioninfoa);

    printf("GetVersionExA Reported Windows version %d.%d.%d platform id %d\n", (int)osversioninfoa.dwMajorVersion,
           (int)osversioninfoa.dwMinorVersion, (int)osversioninfoa.dwBuildNumber, (int)osversioninfoa.dwPlatformId);

    // Let's start with looking at the dwPlatformId value.
    // Possible values are VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS and VER_PLATFORM_WIN32_NT
    // For now, only NT (VER_PLATFORM_WIN32_NT) is supported. Supporting 9x (VER_PLATFORM_WIN32_WINDOWS)
    // will involve some more work due the lack of unicode support.
    // There is unicows.dll but we need to enable that in our binary and all libraries, which means
    // we need a custom runtime.
    // WIN32s (VER_PLATFORM_WIN32s), the 32 bit add-on for Windows 3.x will probably never be supported
    // as it is even more limited. The major limitation is the lack of threading support as Windows 3.x
    // implements cooperative multitasking

    switch (osversioninfoa.dwPlatformId) {
    case VER_PLATFORM_WIN32_NT: {
        // We are on NT

        // We will load libraries manually, to be able to support the lowest Windows version possible.
        // Currently on an MSYS2 MINGW32 build, we can run on XP. (2000 or NT4 haven't been tested)
        HMODULE nldll_library_handle = LoadLibrary("ntdll.dll");
        HMODULE kernel32_library_handle = LoadLibrary("kernel32.dll");
        HMODULE ntoskrnl_library_handle = LoadLibrary("ntoskrnl.exe");

        // We are on a WIN32 platform with PlatformId NT.
        // This could be Microsoft Windows, ReactOS or wine. (are there others?)
        // Let's attempt to detect which it is. First we will see if we are running on ReactOS.
        // ReactOS will put another null-terminated string after the Service Pack
        // See https://reactos.org/forum/viewtopic.php?t=1401
        char reactos[sizeof(osversioninfoa.szCSDVersion)] = {};
        strncpy(reactos, osversioninfoa.szCSDVersion + strlen(osversioninfoa.szCSDVersion) + 1,
                sizeof(osversioninfoa.szCSDVersion) - (strlen(osversioninfoa.szCSDVersion) + 1));

        if (strlen(reactos)) {
            printf("Running on %s\n", reactos);
        } else {
            printf("Not running on ReactOS\n");
        }

        // Another possibility we could be running under wine.
        // Wine exposes several functions in its ntdll.dll
        // See
        // 	* https://gist.github.com/klange/282e62fdd1c2cae6a210443ccbc5b12f
        //  * https://gist.github.com/ork/32da69687c94530931ed

        typedef const char *(*wine_get_version_f)(void);
        typedef const char *(*wine_get_build_id_f)(void);
        typedef const char *(*wine_get_host_version_f)(const char **sysname, const char **release);

        wine_get_version_f wine_get_version = nullptr;
        wine_get_host_version_f wine_get_host_version = nullptr;
        wine_get_build_id_f wine_get_build_id = nullptr;

        if (nldll_library_handle) {
            wine_get_version = (wine_get_version_f)GetProcAddress(nldll_library_handle, "wine_get_version");
            wine_get_host_version = (wine_get_host_version_f)GetProcAddress(nldll_library_handle, "wine_get_host_version");
            if (wine_get_version && wine_get_host_version) {
                const char *sysname;
                const char *version;
                wine_get_host_version(&sysname, &version);
                fprintf(stdout, "Running Wine %s under %s %s.\n", wine_get_version(), sysname, version);
                // Outputs "Running Wine 10.8 under Linux 6.14.6-arch1-1."
                wine_get_build_id = (wine_get_build_id_f)GetProcAddress(nldll_library_handle, "wine_get_build_id");
                if (wine_get_build_id) {
                    fprintf(stdout, "Build ID: %s.\n", wine_get_build_id());
                }
            } else {
                fprintf(stdout, "Not running under wine.\n");
            }
        }

        // If we are running on Microsoft Windows version 10, and the application has no
        // so-called 'manifest', an XML file embedded as 'resource', the version returned
        // by the GetVersionExA is fake. In this case it will report NT 6.2 (Windows 8)

        // Note that MSYS2 automatically adds this manifest to binaries built with it
        // and thus the Windows API behaves as expected, returning the correct version number.
        // However, binaries cross-compiled from Linux (ArchLinux, mingw-w64-toolchain)
        // do not include such manifest. While it is possible to adjust the build system
        // to add such manifest. For now, it is not there.

        // One way around this is to query the NT kernel directly. Note this only
        // is possible when the architecture of this binary and the kernel match.
        // eg. both need to be x86_64. If we are running an i686 binary it will fail.
        // or if we are running an aarch64 kernel.

        RTL_OSVERSIONINFOW rtl_osversioninfow = {0};
        rtl_osversioninfow.dwOSVersionInfoSize = sizeof(rtl_osversioninfow);

        if (ntoskrnl_library_handle) {
            typedef NTSTATUS (*RtlGetVersion_f)(PRTL_OSVERSIONINFOW lpVersionInformation);
            RtlGetVersion_f rtlGetVersion = (RtlGetVersion_f)GetProcAddress(ntoskrnl_library_handle, "RtlGetVersion");
            if (rtlGetVersion) {
                rtlGetVersion(&rtl_osversioninfow);

                printf("RtlGetVersion Reported Windows version %d.%d.%d platform id %d\n", (int)rtl_osversioninfow.dwMajorVersion,
                       (int)rtl_osversioninfow.dwMinorVersion, (int)rtl_osversioninfow.dwBuildNumber,
                       (int)rtl_osversioninfow.dwPlatformId);

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
        // We can query GetSystemInfo and GetNativeSystemInfo.
        // This will correctly identify an i686 binary running on an x86_64 kernel

		// GetNativeSystemInfo is not supported on NT4.
        SYSTEM_INFO system_info = {0};
        GetSystemInfo(&system_info);
        //SYSTEM_INFO native_system_info = {0};
        //GetNativeSystemInfo(&native_system_info);
        printf("GetSystemInfo report ProcessorArchitecture as       %x\n", system_info.wProcessorArchitecture);
        //printf("GetNativeSystemInfo report ProcessorArchitecture as %x\n", native_system_info.wProcessorArchitecture);

        // However, if we are an x86_64 binary running on an aarch64 kernel (supported starting Windows 11)
        // This will not return the expected values. Instead we'll need to call IsWow64Process2()
        // Since this function was added in kernel32.dll in Windows 10, version 1709,
        // we'll need to receive the pointer to the function manually.

        if (kernel32_library_handle) {
            typedef BOOL (*IsWow64Process2_f)(HANDLE hProcess, USHORT *pProcessMachine, USHORT *pNativeMachine);
            IsWow64Process2_f isWow64Process2 = (IsWow64Process2_f)GetProcAddress(kernel32_library_handle, "IsWow64Process2");
            if (isWow64Process2) {
                USHORT ProcessMachine;
                USHORT NativeMachine;
                auto success = isWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine);
                printf("IsWow64Process2 returned %d %X %X\n", success, ProcessMachine, NativeMachine);
            } else {
                puts("IsWow64Process2 not available on this kernel");
            }
        }

        // Another source of version information is the windows registry.
        // However, these values can be edited by the user. So we'll
        // revert to theze values as a last resort.

        // While the most convenient way to get a registry value is RegGetValueA,
        // this function was added in NT 5.2 (Win2003, XP64), we'll settle with a
        // sequence of calls to RegOpenKeyExA RegQueryValueExA RegCloseKey
        // To wrap this sequence, getRegistryDword() and getRegistryString()
        // have been created.

        int status;

        struct {
            int major;
            int minor;
            int patch;
            char sp[128];
            char name[128];
        } registeryVersion;

        // Windows XP:
        // SZ (char*) CurrentVersion, eg "5.1"
        // SZ (char*) CurrentBuildNumber, eg "2600
        // SZ (char*) CSDVersion, eg "Service Pack 3"
        // SZ (char*) ProductName, "Microsoft Windows XP"

        // Windows 10:
        // DWORD (int32_t) CurrentMajorVersionNumber, eg. 10
        // DWORD (int32_t) CurrentMinorVersionNumber, eg. 0
        // SZ (char*)      CurrentBuildNumber, eg. "19045"
        // SZ (char*)      DisplayVersion, eg. "22H2"
        // SZ (char*)      ReleaseId, eg. "2009"
        // SZ (char*)      ProductName, eg. "Windows 10 Pro"

        // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
        // ERROR_FILE_NOT_FOUND

        int32_t CurrentMajorVersionNumber = 0;
        status = getRegistryDword(HKEY_LOCAL_MACHINE,                                // hkey
                                  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                  "CurrentMajorVersionNumber",                       // lpValue
                                  &CurrentMajorVersionNumber);

        printf("Reg CurrentMajorVersionNumber status %x Value %d\n", status, CurrentMajorVersionNumber);
        if (!status) {
            // Windows 10 or later
            registeryVersion.major = CurrentMajorVersionNumber;

            int32_t CurrentMinorVersionNumber = 0;
            status = getRegistryDword(HKEY_LOCAL_MACHINE,                                // hkey
                                      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                      "CurrentMinorVersionNumber",                       // lpValue
                                      &CurrentMinorVersionNumber);
            registeryVersion.minor = CurrentMinorVersionNumber;

            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "DisplayVersion",                                  // lpValue)
                                       registeryVersion.sp,                               // pvData
                                       sizeof(registeryVersion.sp));

        } else {
            // Prior to Windows 10
            char CurrentVersion[32] = {};
            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "CurrentVersion",                                  // lpValue)
                                       CurrentVersion,                                    // pvData
                                       sizeof(CurrentVersion));

            printf("Reg CurrentVersion status %x Value %s\n", status, CurrentVersion);
            sscanf(CurrentVersion, "%d.%d", &registeryVersion.major, &registeryVersion.minor);

            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "CSDVersion",                                      // lpValue)
                                       registeryVersion.sp,                               // pvData
                                       sizeof(registeryVersion.sp));
        }

        char CurrentBuildNumber[32] = {};
        status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                   "CurrentBuildNumber",                              // lpValue)
                                   CurrentBuildNumber,                                // pvData
                                   sizeof(CurrentBuildNumber));
        sscanf(CurrentBuildNumber, "%d", &registeryVersion.patch);

        status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                   "ProductName",                                     // lpValue)
                                   registeryVersion.name,                             // pvData
                                   sizeof(registeryVersion.name));

        // puts("Registry information:");
        // printf("Product Name: %s\n", registeryVersion.name);
        // printf("Version Name: %s\n", registeryVersion.sp);
        // printf("Version     : %d.%d.%d\n", registeryVersion.major, registeryVersion.minor, registeryVersion.patch);

        // Cleaning up
        if (nldll_library_handle)
            FreeLibrary(nldll_library_handle);
        if (kernel32_library_handle)
            FreeLibrary(kernel32_library_handle);
        if (ntoskrnl_library_handle)
            FreeLibrary(ntoskrnl_library_handle);

    } break;

    case VER_PLATFORM_WIN32_WINDOWS: {
        // Running on 9x
    } break;

    case VER_PLATFORM_WIN32s: {
        // Running on WIN32s
        // Unlikely we will ever get here ;)
    } break;
    }
}

#endif
