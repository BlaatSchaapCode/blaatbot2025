#include "osVersion.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#include <dbghelp.h>

#include <cstdint>
#include <string>

int getRegistryDword(HKEY hKey, const char *subkey, const char *value_name, int32_t *value) {
    // RegOpenKeyExA RegQueryValueExA RegCloseKey
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
    OSVERSIONINFOA osversioninfoa = {0};
    osversioninfoa.dwOSVersionInfoSize = sizeof(osversioninfoa);
    GetVersionExA(&osversioninfoa);

    printf("GetVersionExA Reported Windows version %d.%d.%d platform id %d\n", osversioninfoa.dwMajorVersion,
           osversioninfoa.dwMinorVersion, osversioninfoa.dwBuildNumber, osversioninfoa.dwPlatformId);

    /* dwPlatformId values:
                VER_PLATFORM_WIN32s
                VER_PLATFORM_WIN32_WINDOWS
                VER_PLATFORM_WIN32_NT
     */

    switch (osversioninfoa.dwPlatformId) {
    case VER_PLATFORM_WIN32_NT: {

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

        // Checking for wine
        // https://gist.github.com/klange/282e62fdd1c2cae6a210443ccbc5b12f
        // https://gist.github.com/ork/32da69687c94530931ed

        typedef const char *(*wine_get_version_f)(void);
        typedef const char *(*wine_get_build_id_f)(void);
        typedef const char *(*wine_get_host_version_f)(const char **sysname, const char **release);

        wine_get_version_f wine_get_version = nullptr;
        wine_get_host_version_f wine_get_host_version = nullptr;
        wine_get_build_id_f wine_get_build_id = nullptr;

        HMODULE nldll_library_handle = LoadLibrary("ntdll.dll");
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
            FreeLibrary(nldll_library_handle);
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
		
		// We need to disable redirection if we want to capture the true version.
		// see https://learn.microsoft.com/en-us/windows/win32/api/wow64apiset/nf-wow64apiset-wow64disablewow64fsredirection
		// Wow64DisableWow64FsRedirection
		// Wow64RevertWow64FsRedirection
		// Note: we need to get the function from the dll to be compatible with 32 bit xp.
		// Note: we need to verify this also affects when running x86_64 binaries on an aarch64 kernel
		//       as this is what we want to detect.
		// Note: Also test if IsWow64Process2 could provide the answer.
		
		
		// Min version Windows 10, version 1709
		auto kernel32_library_handle = LoadLibrary("kernel32.dll");
		if (kernel32_library_handle) {
			typedef BOOL (*IsWow64Process2_f)(HANDLE hProcess, USHORT *pProcessMachine, USHORT *pNativeMachine);		
			IsWow64Process2_f isWow64Process2 = (IsWow64Process2_f)GetProcAddress(kernel32_library_handle, "IsWow64Process2");
			if (isWow64Process2)  {
				USHORT ProcessMachine;
				USHORT NativeMachine;
				auto success = isWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine);
				printf("IsWow64Process2 returned %d %X %X\n", success, ProcessMachine, NativeMachine);
			} else {
				puts("IsWow64Process2 not availble on this kernel");
			}
		}
				
				

        auto kernel_library_handle = LoadLibrary("ntoskrnl.exe");
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
            FreeLibrary(kernel_library_handle);
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

        HANDLE hFile = CreateFileW(windowsKernelPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY, NULL);
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
        LSTATUS status;

        struct {
            int major;
            int minor;
            int patch;
            char sp[128];
            char name[128];
        } registeryVersion;

        // Note RegGetValueA
        // Requirements
        // Minimum supported client 	Windows Vista, Windows XP Professional x64 Edition
        // Minimum supported server 	Windows Server 2008, Windows Server 2003 with SP1
        // Thus to support XP, we need the RegOpenKeyExA RegQueryValueExA RegCloseKey

        // Windows XP:
        // SZ CurrentVersion
        // SZ CurrentBuildNumber
        // SZ CSDVersion
        // SZ ProductName

        // Windows 10:
        // DWORD CurrentMajorVersionNumber;
        // DWORD CurrentMinorVersionNumber;
        // SZ CurrentBuildNumber
        // SZ DisplayVersion
        // SZ ProductName

        // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
        // ERROR_FILE_NOT_FOUND
        // DWORD CurrentMajorVersionNumber = 0;
        // pcbData = sizeof(CurrentMajorVersionNumber);
        // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
        //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
        //                       "CurrentMajorVersionNumber",                       // lpValue
        //                       RRF_RT_DWORD | RRF_ZEROONFAILURE,                  // dwFlags
        //                       nullptr,                                           // pdwType
        //                       &CurrentMajorVersionNumber,                        // pvData
        //                       &pcbData                                           // pcbData

        int32_t CurrentMajorVersionNumber = 0;
        status = getRegistryDword(HKEY_LOCAL_MACHINE,                                // hkey
                                  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                  "CurrentMajorVersionNumber",                       // lpValue
                                  &CurrentMajorVersionNumber);

        printf("Reg CurrentMajorVersionNumber status %x Value %d\n", status, CurrentMajorVersionNumber);
        if (!status) {
            // Windows 10 or later
            registeryVersion.major = CurrentMajorVersionNumber;

            // DWORD CurrentMinorVersionNumber = 0;
            // pcbData = sizeof(CurrentMinorVersionNumber);
            // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
            //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
            //                       "CurrentMinorVersionNumber",                       // lpValue
            //                       RRF_RT_DWORD | RRF_ZEROONFAILURE,                  // dwFlags
            //                       nullptr,                                           // pdwType
            //                       &CurrentMinorVersionNumber,                        // pvData
            //                       &pcbData                                           // pcbData
            // );

            int32_t CurrentMinorVersionNumber = 0;
            status = getRegistryDword(HKEY_LOCAL_MACHINE,                                // hkey
                                      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                      "CurrentMinorVersionNumber",                       // lpValue
                                      &CurrentMinorVersionNumber);
            registeryVersion.minor = CurrentMinorVersionNumber;

            // pcbData = sizeof(registeryVersion.sp);
            // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
            //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
            //                       "DisplayVersion",                                  // lpValue
            //                       RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
            //                       nullptr,                                           // pdwType
            //                       registeryVersion.sp,                              // pvData
            //                       &pcbData                                           // pcbData
            //);

            // int getRegistryString(HKEY hKey, const char *subkey, const char *value_name, char *value, size_t len)
            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "DisplayVersion",                                  // lpValue)
                                       registeryVersion.sp,                               // pvData
                                       sizeof(registeryVersion.sp));

        } else {
            // Prior to Windows 10
            char CurrentVersion[32] = {};
            // pcbData = sizeof(CurrentVersion);
            // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
            //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
            //                       "CurrentVersion",                                  // lpValue
            //                       RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
            //                       nullptr,                                           // pdwType
            //                       &CurrentVersion,                                   // pvData
            //                       &pcbData                                           // pcbData
            //);
            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "CurrentVersion",                                  // lpValue)
                                       CurrentVersion,                                    // pvData
                                       sizeof(CurrentVersion));

            printf("Reg CurrentVersion status %x Value %s\n", status, CurrentVersion);
            sscanf(CurrentVersion, "%d.%d", &registeryVersion.major, &registeryVersion.minor);

            // pcbData = sizeof(registeryVersion.sp);
            // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
            //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
            //                       "CSDVersion",                                      // lpValue
            //                       RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
            //                       nullptr,                                           // pdwType
            //                       registeryVersion.sp,                              // pvData
            //                       &pcbData                                           // pcbData
            //);
            status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                       "CSDVersion",                                      // lpValue)
                                       registeryVersion.sp,                               // pvData
                                       sizeof(registeryVersion.sp));
        }

        char CurrentBuildNumber[32] = {};
        // pcbData = sizeof(CurrentBuildNumber);
        // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
        //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
        //                       "CurrentBuildNumber",                              // lpValue
        //                       RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
        //                       nullptr,                                           // pdwType
        //                       &CurrentBuildNumber,                               // pvData
        //                       &pcbData                                           // pcbData
        //);
        status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                   "CurrentBuildNumber",                              // lpValue)
                                   CurrentBuildNumber,                                // pvData
                                   sizeof(CurrentBuildNumber));
        sscanf(CurrentBuildNumber, "%d", &registeryVersion.patch);

        pcbData = sizeof(registeryVersion.name);
        // status = RegGetValueA(HKEY_LOCAL_MACHINE,                                // hkey
        //                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
        //                       "ProductName",                                     // lpValue
        //                       RRF_RT_REG_SZ | RRF_ZEROONFAILURE,                 // dwFlags
        //                       nullptr,                                           // pdwType
        //                       &registeryVersion.name,                            // pvData
        //                       &pcbData                                           // pcbData
        //);
        status = getRegistryString(HKEY_LOCAL_MACHINE,                                // hkey
                                   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", // lpSubKey
                                   "ProductName",                                     // lpValue)
                                   registeryVersion.name,                             // pvData
                                   sizeof(registeryVersion.name));

        puts("Registry information:");
        printf("Product Name: %s\n", registeryVersion.name);
        printf("Version Name: %s\n", registeryVersion.sp);
        printf("Version     : %d.%d.%d\n", registeryVersion.major, registeryVersion.minor, registeryVersion.patch);

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
