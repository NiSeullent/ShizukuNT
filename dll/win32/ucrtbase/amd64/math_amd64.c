/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     x64 FMA3 library state and _except1 control word decoding, after Wine 11.21 dlls/msvcrt/math.c
 * COPYRIGHT:   Copyright 2000 Jon Griffiths
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <fpieee.h>
#include <windef.h>

extern int __use_fma3_lib;

double __cdecl __ucrtbase_except1(DWORD fpe, _FP_OPERATION_CODE op, double res, DWORD cw);

int __cdecl _get_FMA3_enable(void)
{
    return __use_fma3_lib;
}

double __cdecl _except1(DWORD fpe, _FP_OPERATION_CODE op, double arg, double res, DWORD cw, void *unk)
{
    (void)arg;
    (void)unk;
    return __ucrtbase_except1(fpe, op, res, ((cw >> 7) & 0x3f) | ((cw >> 3) & 0xc00));
}
