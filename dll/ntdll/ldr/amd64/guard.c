/*
 * PROJECT:     LiberNT NT Layer DLL
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     AMD64 Control Flow Guard loader targets
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <ntdll.h>

VOID NTAPI LdrpGuardCheckIcall(VOID);
VOID NTAPI LdrpGuardDispatchIcall(VOID);

BOOLEAN
LdrpArchGetGuardTargets(
    _Out_ PVOID *CheckTarget,
    _Out_ PVOID *DispatchTarget)
{
    *CheckTarget = (PVOID)LdrpGuardCheckIcall;
    *DispatchTarget = (PVOID)LdrpGuardDispatchIcall;
    return TRUE;
}
