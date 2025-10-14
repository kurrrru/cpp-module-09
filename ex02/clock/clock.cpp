#pragma once

#include <ex02/clock/clock.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#endif

int64 get_microseconds() {
#ifdef _WIN32
    LARGE_INTEGER freq;
    if (!QueryPerformanceFrequency(&freq)) {
        return 0;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<int64>(counter.QuadPart * 1000000) / freq.QuadPart;

#elif defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return static_cast<int64>(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;

#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<int64>(tv.tv_sec) * 1000000 + tv.tv_usec;
#endif
}
