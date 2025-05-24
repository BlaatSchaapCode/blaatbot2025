
#include "time.hpp"

#if __cpp_lib_chrono >= 201907L
#include <chrono>

std::string getTimeString(void) {
    const auto zt{std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}};
        return  std::format("{:%FT%T%z}", zt));
}

#elif defined __MSVCRT__
// If we are using MSVCRT in stead of UCRT, we use a C89 library
// with no nice date formatting functions.

// Note: when building under MSYS64 UCRT64 this code path also gets taken
// despite we are targetting ucrt in stead of msvcrt.

#include <sys/time.h>
std::string getTimeString(void) {
	std::string result = "no time";
	struct timeval timeval = {};
	struct timezone timezone  = {};

	auto res = gettimeofday(&timeval, &timezone);

	if (timezone.tz_dsttime) {
		// Is this an MSVCRT bug or expected behaviour. The time zone
		// does not appear to include DST???
		if (timezone.tz_minuteswest < 0) timezone.tz_minuteswest -= 60;
		else timezone.tz_minuteswest += 60;
	}

	time_t now = time(nullptr);
	auto tm  = localtime ( &now);

	// 2025-05-24T23:40:24+0200
	if (tm) {
		char buff[64] = {};
		snprintf(buff, sizeof(buff), "%04d-%02d-%02dT%02d:%02d:%02d%c%02d%02d",
				tm->tm_year + 1900, // year
				tm->tm_mon + 1, // month
				tm->tm_mday, // day
				tm->tm_hour, // hour
				tm->tm_min, // minute
				tm->tm_sec, // second
				timezone.tz_minuteswest > 0 ? '-' : '+', // plus or minus
				abs(timezone.tz_minuteswest) / 60, // timezone hour
				abs(timezone.tz_minuteswest) % 60 // timezone minute
				);
		result = buff;

	}
	return result;
}

#else
#include <ctime>
std::string getTimeString(void) {
    // When there is no C++ support for getting time time with time zone, use C.
	// This code path gets taken on Haiku.
    char buff[64] = {};
    time_t current_time = time(nullptr);
    strftime(buff, sizeof(buff), "%FT%T%z", localtime(&current_time));
    return buff;
}

#endif
