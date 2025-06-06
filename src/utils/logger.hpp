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

#pragma once

#include <cstdio>
#include <string_view>

#ifndef __FILENAME__
#define __FILENAME__ "unknown"
// Are you using the correct Makefiles????
#endif

/*
 * How to use logging:
 * For logging we use printf like macro's.
 * Most of the logging should be used with LOG, WARNING and ERROR, but there are
 * also two special ones: DBG_LOG and TEST_LOG
 * - LOG:      Anything that is relevant when reading the logs but is not odd.
 * - WARNING:  Something odd but not nessesary a bug.
 * - ERROR:    A runtime detected problem.
 * - DBG_LOG:  A log message relevant when debugging certain subsystems. These
 * are usually disabled but can be enabled on a file basis.
 * - TEST_LOG: A log message specifically for tests. They will not compile in
 * release, forcing the user to remove it.
 */

#ifdef _MSC_FULL_VER
#define LOG_INFO(format, ...) utils::logger::log_impl(utils::logger::LogLevel::Info, __FILENAME__, __LINE__, format, __VA_ARGS__)
#define LOG_WARNING(format, ...)                                                                                                   \
    utils::logger::log_impl(utils::logger::LogLevel::Warning, __FILENAME__, __LINE__, format, __VA_ARGS__)
#define LOG_ERROR(format, ...) utils::logger::log_impl(utils::logger::LogLevel::Error, __FILENAME__, __LINE__, format, __VA_ARGS__)
#else
#define LOG_INFO(format, ...)                                                                                                      \
    utils::logger::log_impl(utils::logger::LogLevel::Info, __FILENAME__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARNING(format, ...)                                                                                                   \
    utils::logger::log_impl(utils::logger::LogLevel::Warning, __FILENAME__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(format, ...)                                                                                                     \
    utils::logger::log_impl(utils::logger::LogLevel::Error, __FILENAME__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifdef ENABLE_LOG_DEBUG
#define LOG_DEBUG(format, ...)                                                                                                     \
    utils::logger::log_impl(utils::logger::LogLevel::DebugLog, __FILENAME__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif

#ifdef _DEBUG
#define LOG_TEST(format, ...)                                                                                                      \
    utils::logger::log_impl(utils::logger::LogLevel::TestLog, __FILENAME__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#endif

namespace utils::logger {
enum class LogLevel {
    Info,
    TestLog,
    DebugLog,
    Warning,
    Error,
};

void log_impl2(LogLevel level, const char *file, int line, std::string_view msg);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#endif
// Clang (version 13.0.1) does not play nice with
//		format __VA_OPT__(,)
// in the LOG_ macros when there are no parameters to the log
// That is exactly when __VA_OPT__(,) comes into play, to allow such
// But then clang will errourously generate a
//		error: format string is not a string literal (potentially
// insecure) [-Werror,-Wformat-security]
// error, even though the format is a literal, it apparently gets confused by
// the __VA_OPT__
inline void log_impl(LogLevel level, const char *file, int line, const char *format, auto... args) {
    const int MAX_BUFFER = 1024;
    char buffer[MAX_BUFFER];
    snprintf(buffer, MAX_BUFFER, format, args...);
    log_impl2(level, file, line, buffer);
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace utils::logger
