/*
 * PROJECT:     LiberNT TI-RPC library
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     BSD gettimeofday() on top of the Win32 system time
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <wintirpc.h>
#include <time.h>

#define TICKS_PER_SECOND        10000000ULL
#define TICKS_PER_MICROSECOND   10ULL
#define TICKS_1601_TO_1970      116444736000000000ULL

int
gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (tv != NULL)
    {
        FILETIME SystemTime;
        ULARGE_INTEGER Ticks;

        GetSystemTimeAsFileTime(&SystemTime);
        Ticks.LowPart = SystemTime.dwLowDateTime;
        Ticks.HighPart = SystemTime.dwHighDateTime;
        Ticks.QuadPart -= TICKS_1601_TO_1970;

        tv->tv_sec = (long)(Ticks.QuadPart / TICKS_PER_SECOND);
        tv->tv_usec = (long)((Ticks.QuadPart % TICKS_PER_SECOND) / TICKS_PER_MICROSECOND);
    }

    if (tz != NULL)
    {
        _tzset();
        tz->tz_minuteswest = (int)(_timezone / 60);
        tz->tz_dsttime = _daylight;
    }

    return 0;
}
