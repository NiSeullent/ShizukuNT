/*
 * PROJECT:     ReactOS API tests
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     CFG-instrumented loader test image
 */

#include <windows.h>

#if defined(_M_IX86) || defined(_M_AMD64)
__declspec(dllexport) __attribute__((naked))
DWORD WINAPI
CfgGuardedTarget(VOID)
{
    __asm__ __volatile__("movl $0x47f, %eax\n\tret");
}
#else
__declspec(dllexport) __declspec(noinline)
DWORD WINAPI
CfgGuardedTarget(VOID)
{
    return 0x47f;
}
#endif

__declspec(dllexport) __declspec(noinline) __attribute__((section(".cfgx1")))
DWORD WINAPI
CfgGuardedSection1(VOID)
{
    return 0x481;
}

__declspec(dllexport) __declspec(noinline) __attribute__((section(".cfgx2")))
DWORD WINAPI
CfgGuardedSection2(VOID)
{
    return 0x482;
}

__declspec(dllexport) __declspec(noinline) __attribute__((section(".cfgx3")))
DWORD WINAPI
CfgGuardedSection3(VOID)
{
    return 0x483;
}

__declspec(dllexport) __declspec(noinline) __attribute__((section(".cfgx4")))
DWORD WINAPI
CfgGuardedSection4(VOID)
{
    return 0x484;
}

BOOL WINAPI
DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(Reason);
    UNREFERENCED_PARAMETER(Reserved);
    return TRUE;
}
