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

#include "Logger.hpp"

/*
 * How to use logging:
 * For logging we use printf like macro's.
 * Most of the logging should be used with LOG, WARNING and ERROR, but there are
 * also two special ones: DBG_LOG and TEST_LOG
 * - LOG:      Anything that is relevant when reading the logs but is not odd.
 * - WARNING:  Something odd but not nessesary a bug.
 * - ERROR:    A runtime detected problem.
 * - DEBUG:  A log message relevant when debugging certain subsystems. These
 * are usually disabled but can be enabled on a file basis.
 */

#define LOG_INFO(format, ...)                                                                                                      \
    geblaat::log_impl(geblaat::Logger::LogLevel::Info, MODULE, __FILENAME__, __LINE__, __FUNCTION__,                               \
                      format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARNING(format, ...)                                                                                                   \
    geblaat::log_impl(geblaat::Logger::LogLevel::Warning, MODULE, __FILENAME__, __LINE__, __FUNCTION__,                            \
                      format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(format, ...)                                                                                                     \
    geblaat::log_impl(geblaat::Logger::LogLevel::Error, MODULE, __FILENAME__, __LINE__, __FUNCTION__,                              \
                      format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG(format, ...)                                                                                                     \
    geblaat::log_impl(geblaat::Logger::LogLevel::Debug, MODULE, __FILENAME__, __LINE__, __FUNCTION__,                              \
                      format __VA_OPT__(, ) __VA_ARGS__)

namespace geblaat {

// void log_impl2(Logger::LogLevel level, const char * module, const char *filename, unsigned line, std::string_view msg);
void log_impl2(geblaat::Logger::LogLevel level, const char *module, const char *filename, unsigned line, const char *function,
               const char *message);

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
inline void log_impl(Logger::LogLevel level, const char *module, const char *filename, int line, const char *function,
                     const char *format, auto... args) {
    const int MAX_BUFFER = 1024;
    char buffer[MAX_BUFFER];
    snprintf(buffer, MAX_BUFFER, format, args...);
    log_impl2(level, module, filename, line, function, buffer);
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace geblaat
