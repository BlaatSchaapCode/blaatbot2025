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

#include "executablePath.hpp"

#include <iostream>

// https://stackoverflow.com/questions/933850/how-do-i-find-the-location-of-the-executable-in-c

#if defined __linux__
#include <linux/limits.h>
#define LINK_TO_SELF "/proc/self/exe"
#endif

#if defined __FreeBSD__
#include <sys/syslimits.h>
#define LINK_TO_SELF "/proc/curproc/file"
#endif

#if defined __illumos__
#include <limits.h>
#define LINK_TO_SELF "/proc/self/path/a.out"
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <shlwapi.h>
#include <windows.h>

std::string getExecutablePath() {
    std::string result;
    wchar_t executablePath[PATH_MAX + 1] = {};
    // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
    auto res = GetModuleFileNameW(nullptr, executablePath, PATH_MAX);
    if (res > 0) {
        char buf[PATH_MAX + 1] = {};
        // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
        res = WideCharToMultiByte(CP_UTF8, 0, executablePath, -1, buf, sizeof(buf), NULL, NULL);
        if (res > 0) {
            result = buf;
        }
    }
    return result;
}

std::string getPluginDir(void) {
    std::string result;
    wchar_t pluginDir[PATH_MAX + 1] = {};
    wchar_t executablePath[PATH_MAX + 1] = {};
    // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
    auto res = GetModuleFileNameW(nullptr, executablePath, PATH_MAX);
    if (res > 0) {
        // https://learn.microsoft.com/nl-nl/windows/win32/api/shlwapi/nf-shlwapi-pathremovefilespecw
        PathRemoveFileSpecW(executablePath);
        std::wstring pluginDirStr = (std::wstring)(executablePath) + std::wstring(L"\\..\\lib");
        res = GetFullPathNameW(pluginDirStr.c_str(), PATH_MAX, pluginDir, nullptr);
        if (res > 0) {
            char buf[PATH_MAX + 1] = {};
            // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
            res = WideCharToMultiByte(CP_UTF8, 0, pluginDir, -1, buf, sizeof(buf), nullptr, nullptr);
            if (res > 0) {
                result = buf;
            }
        }
    }
    return result;
}

#elif defined __HAIKU__
#include <kernel/image.h>
std::string getExecutablePath() {
    std::string result;
    int32 cookie = 0;
    image_info info;
    while (get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK) {
        if (info.type == B_APP_IMAGE) {
            result = info.name;
        }
    }
    return result;
}

#elif defined LINK_TO_SELF
#include <unistd.h>

std::string getExecutablePath(void) {
    char buffer[PATH_MAX + 1] = {};
    auto result = readlink(LINK_TO_SELF, buffer, PATH_MAX);
    if (result < 0) {
        // error
        return "";
    }
    return buffer;
}

#else
// Unknown/Unsupported OS
std::string getExecutablePath(void) { return ""; }
#endif

#if defined LINK_TO_SELF || defined __HAIKU__
#include <libgen.h>
std::string getPluginDir(void) {
    std::string result;
    std::string executablePath = getExecutablePath();

    // manpage basename(3) states:
    // Both dirname() and basename() return pointers to null-terminated strings.
    // (Do not pass these pointers to free(3).)
    std::string executableDir = dirname((char *)executablePath.c_str());

    std::string libraryDir = executableDir + "/../lib/";
    // manpage realpath(3) states:
    // If  resolved_path  is  specified  as  NULL,  then  realpath() uses
    // malloc(3) to allocate a buffer of up to PATH_MAX bytes to hold the resolved
    // pathname, and returns a pointer to this buffer.  The caller should
    // deallocate this buffer using free(3).
    char *realLibraryPath = realpath((char *)libraryDir.c_str(), nullptr);
    if (realLibraryPath) {
        result = realLibraryPath;
        free(realLibraryPath);
    }
    return result;
}
#elif !(defined(_WIN32) || defined(_WIN64))
// Unknown/Unsupported OS
std::string getPluginDir(void) { return "../lib"; }
#endif
