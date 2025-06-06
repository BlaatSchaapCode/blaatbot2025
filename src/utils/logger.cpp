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

#include "logger.hpp"

#include "time.hpp"
#include <cstring>


namespace utils::logger {
// template <size_t N> static consteval size_t compile_time_strlen(char const
// (&)[N]) { return N - 1; } const constexpr size_t FILE_SKIP =
// compile_time_strlen(__FILE__) - compile_time_strlen("utils/logger.cpp");
//// Note: FILE_SKIP is machine dependent

const char *get_label(LogLevel level) {
    switch (level) {
    case LogLevel::Warning:
        return "[WARNING] ";
    case LogLevel::Error:
        return "[ERROR]   ";
    case LogLevel::DebugLog:
        return "[DEBUG]   ";
    case LogLevel::TestLog:
        return "[TEST]    ";
    default:
        return "[LOG]     ";
    }
}

void log_impl2(LogLevel level, const char *file, int line, std::string_view msg) {
    printf("%s %s%16s:%-4d %s\n", getTimeString().c_str(), get_label(level), file, line, msg.data());

    // TODO: Also write the log to a file
}
} // namespace utils::logger
