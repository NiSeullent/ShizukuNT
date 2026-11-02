/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the TI-RPC library gettimeofday()
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <wintirpc.h>
#include <time.h>
#include <apitest.h>

static unsigned long long
FileTimeToUnixMicroseconds(const FILETIME *FileTime)
{
    ULARGE_INTEGER Ticks;

    Ticks.LowPart = FileTime->dwLowDateTime;
    Ticks.HighPart = FileTime->dwHighDateTime;
    return (Ticks.QuadPart - 116444736000000000ULL) / 10;
}

static void
TestZone(const char *Setting, int MinutesWest, int DstTime)
{
    struct timezone Zone;
    char Variable[32];

    sprintf(Variable, "TZ=%s", Setting);
    ok_int(_putenv(Variable), 0);

    memset(&Zone, 0xcc, sizeof(Zone));
    ok_int(gettimeofday(NULL, &Zone), 0);
    ok_int(Zone.tz_minuteswest, MinutesWest);
    ok_int(Zone.tz_dsttime, DstTime);
}

START_TEST(GetTimeOfDay)
{
    struct timeval Now;
    FILETIME Before, After;
    unsigned long long BeforeUs, AfterUs, NowUs;
    long long CrtSeconds, Difference;

    memset(&Now, 0xcc, sizeof(Now));
    GetSystemTimeAsFileTime(&Before);
    ok_int(gettimeofday(&Now, NULL), 0);
    GetSystemTimeAsFileTime(&After);
    CrtSeconds = (long long)time(NULL);

    ok(Now.tv_usec >= 0 && Now.tv_usec < 1000000, "tv_usec = %ld\n", Now.tv_usec);
    BeforeUs = FileTimeToUnixMicroseconds(&Before);
    AfterUs = FileTimeToUnixMicroseconds(&After);
    NowUs = (unsigned long long)Now.tv_sec * 1000000 + (unsigned long long)Now.tv_usec;
    ok(NowUs >= BeforeUs && NowUs <= AfterUs, "%I64u not in [%I64u, %I64u]\n", NowUs, BeforeUs, AfterUs);

    Difference = CrtSeconds - (long long)Now.tv_sec;
    ok(Difference >= 0 && Difference <= 1, "time() - tv_sec = %I64d\n", Difference);

    ok_int(gettimeofday(NULL, NULL), 0);

    TestZone("PST8PDT", 480, 1);
    TestZone("GMT0", 0, 0);
    TestZone("JST-9", -540, 0);

    ok_int(_putenv("TZ="), 0);
    _tzset();
}
