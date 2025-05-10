
#include "time.hpp"

#if __cpp_lib_chrono >= 201907L
#include <chrono>

std::string getTimeString(void) {
    const auto zt{std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}};
        return  std::format("{:%FT%T%z}", zt));
}

#else
#include <ctime>
std::string getTimeString(void) {
    // When there is no C++ support for getting time time with time zone, use C.
    char buff[80] = {};
    time_t current_time = time(nullptr);
    strftime(buff, sizeof(buff), "%FT%T%z", localtime(&current_time));
    return buff;
}

#endif
