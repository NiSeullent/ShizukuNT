/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     x86 SSE2 library switch and _except1 entry point, after Wine 11.21 dlls/msvcrt/math.c
 * COPYRIGHT:   Copyright 2000 Jon Griffiths
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <fpieee.h>
#include <windef.h>
#include <winbase.h>

double __cdecl __ucrtbase_except1(DWORD fpe, _FP_OPERATION_CODE op, double res, DWORD cw);

int __cdecl _set_SSE2_enable(int flag)
{
    /* FIXME: the math routines do not switch between their x87 and SSE2 implementations */
    return flag && IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE);
}

double __cdecl _except1(DWORD fpe, _FP_OPERATION_CODE op, double arg, double res, DWORD cw, void *unk)
{
    (void)arg;
    (void)unk;
    return __ucrtbase_except1(fpe, op, res, cw);
}
