/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     .NET Framework 4.5 and later setup registry values that applications query
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#define WIN32_NO_STATUS
#define _INC_WINDOWS
#define COM_NO_WINDOWS_H

#include <apitest.h>
#include <winreg.h>
#include <ntstatus.h>
#include <ndk/rtlfuncs.h>

#define NDP_V4_FULL L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"
#define NDP_V45_RELEASE 378389

static DWORD
ExpectedRelease(DWORD BuildNumber)
{
    switch (BuildNumber)
    {
        case 22621:
        case 22631:
            return 533320;
        case 26100:
            return 533325;
        case 26200:
            return 533509;
        default:
            return 0;
    }
}

static void
TestView(PCSTR Name, REGSAM View, DWORD BuildNumber)
{
    HKEY Key;
    LONG Error;
    DWORD Type, Size, Value, Expected;
    WCHAR Version[64];

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, NDP_V4_FULL, 0, KEY_QUERY_VALUE | View, &Key);
    if (Error == ERROR_FILE_NOT_FOUND)
    {
        skip("%s: .NET Framework 4.5 or later is not installed\n", Name);
        return;
    }
    ok(Error == ERROR_SUCCESS, "%s: RegOpenKeyExW returned %ld\n", Name, Error);
    if (Error != ERROR_SUCCESS)
        return;

    Value = 0xdeadbeef;
    Size = sizeof(Value);
    Error = RegQueryValueExW(Key, L"Install", NULL, &Type, (PBYTE)&Value, &Size);
    ok(Error == ERROR_SUCCESS, "%s: Install query returned %ld\n", Name, Error);
    ok(Type == REG_DWORD, "%s: Install type is %lu\n", Name, Type);
    ok(Size == sizeof(DWORD), "%s: Install size is %lu\n", Name, Size);
    ok(Value == 1, "%s: Install is %lu\n", Name, Value);

    Value = 0xdeadbeef;
    Size = sizeof(Value);
    Error = RegQueryValueExW(Key, L"Release", NULL, &Type, (PBYTE)&Value, &Size);
    ok(Error == ERROR_SUCCESS, "%s: Release query returned %ld\n", Name, Error);
    ok(Type == REG_DWORD, "%s: Release type is %lu\n", Name, Type);
    ok(Size == sizeof(DWORD), "%s: Release size is %lu\n", Name, Size);
    Expected = ExpectedRelease(BuildNumber);
    if (Expected)
        ok(Value == Expected, "%s: Release is %lu, expected %lu on build %lu\n", Name, Value, Expected, BuildNumber);
    else
        ok(Value >= NDP_V45_RELEASE && Value != 0xdeadbeef, "%s: Release is %lu\n", Name, Value);

    Size = sizeof(Version);
    Error = RegQueryValueExW(Key, L"Version", NULL, &Type, (PBYTE)Version, &Size);
    ok(Error == ERROR_SUCCESS, "%s: Version query returned %ld\n", Name, Error);
    ok(Type == REG_SZ, "%s: Version type is %lu\n", Name, Type);
    if (Error == ERROR_SUCCESS && Type == REG_SZ && Size >= sizeof(WCHAR))
    {
        Version[min(Size / sizeof(WCHAR), ARRAYSIZE(Version)) - 1] = UNICODE_NULL;
        ok(wcsncmp(Version, L"4.", 2) == 0, "%s: Version is '%S'\n", Name, Version);
    }

    RegCloseKey(Key);
}

START_TEST(NetFrameworkSetup)
{
    RTL_OSVERSIONINFOW Info = { sizeof(Info) };
    NTSTATUS Status;

    Status = RtlGetVersion(&Info);
    ok(Status == STATUS_SUCCESS, "RtlGetVersion returned 0x%lx\n", Status);
    trace("Windows %lu.%lu.%lu\n", Info.dwMajorVersion, Info.dwMinorVersion, Info.dwBuildNumber);

    TestView("Native view", KEY_WOW64_64KEY, Info.dwBuildNumber);
    TestView("32-bit view", KEY_WOW64_32KEY, Info.dwBuildNumber);
}
