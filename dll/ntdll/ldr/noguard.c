/*
 * PROJECT:     LiberNT NT Layer DLL
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Loader hook for architectures without Control Flow Guard targets
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <ntdll.h>

BOOLEAN
LdrpArchGetGuardTargets(
    _Out_ PVOID *CheckTarget,
    _Out_ PVOID *DispatchTarget)
{
    *CheckTarget = NULL;
    *DispatchTarget = NULL;
    return FALSE;
}
