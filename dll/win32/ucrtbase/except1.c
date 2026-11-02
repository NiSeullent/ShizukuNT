/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     _except1 floating-point error dispatch from Wine 11.21 dlls/msvcrt/math.c
 * COPYRIGHT:   Copyright 2000 Jon Griffiths
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <float.h>
#include <fpieee.h>
#include <math.h>
#include <stdint.h>
#include <windef.h>
#include <winbase.h>
#include <xmmintrin.h>

void _set_statfp(uintptr_t mask);

double __cdecl __ucrtbase_except1(DWORD fpe, _FP_OPERATION_CODE op, double res, DWORD cw)
{
    ULONG_PTR exception_arg;
    DWORD exception = 0;
    unsigned int fpword = 0;
    WORD operation;
    uintptr_t raise = 0;

    operation = op << 5;
    exception_arg = (ULONG_PTR)&operation;

    if (fpe & 0x1) {
        if ((fpe == 0x1 && (cw & 0x8)) || (fpe==0x11 && (cw & 0x28))) {
            raise |= _MM_EXCEPT_OVERFLOW;
            if (fpe & 0x10) raise |= _MM_EXCEPT_INEXACT;
            res = signbit(res) ? -INFINITY : INFINITY;
        } else {
            exception = EXCEPTION_FLT_OVERFLOW;
        }
    } else if (fpe & 0x2) {
        if ((fpe == 0x2 && (cw & 0x10)) || (fpe==0x12 && (cw & 0x30))) {
            raise |= _MM_EXCEPT_UNDERFLOW;
            if (fpe & 0x10) raise |= _MM_EXCEPT_INEXACT;
            res = signbit(res) ? -0.0 : 0.0;
        } else {
            exception = EXCEPTION_FLT_UNDERFLOW;
        }
    } else if (fpe & 0x4) {
        if ((fpe == 0x4 && (cw & 0x4)) || (fpe==0x14 && (cw & 0x24))) {
            raise |= _MM_EXCEPT_DIV_ZERO;
            if (fpe & 0x10) raise |= _MM_EXCEPT_INEXACT;
        } else {
            exception = EXCEPTION_FLT_DIVIDE_BY_ZERO;
        }
    } else if (fpe & 0x8) {
        if (fpe == 0x8 && (cw & 0x1)) {
            raise |= _MM_EXCEPT_INVALID;
        } else {
            exception = EXCEPTION_FLT_INVALID_OPERATION;
        }
    } else if (fpe & 0x10) {
        if (fpe == 0x10 && (cw & 0x20)) {
            raise |= _MM_EXCEPT_INEXACT;
        } else {
            exception = EXCEPTION_FLT_INEXACT_RESULT;
        }
    }

    if (exception)
        raise = 0;
    _set_statfp(raise);
    if (exception)
        RaiseException(exception, 0, 1, &exception_arg);

    if (cw & 0x1) fpword |= _EM_INVALID;
    if (cw & 0x2) fpword |= _EM_DENORMAL;
    if (cw & 0x4) fpword |= _EM_ZERODIVIDE;
    if (cw & 0x8) fpword |= _EM_OVERFLOW;
    if (cw & 0x10) fpword |= _EM_UNDERFLOW;
    if (cw & 0x20) fpword |= _EM_INEXACT;
    switch (cw & 0xc00)
    {
        case 0xc00: fpword |= _RC_UP|_RC_DOWN; break;
        case 0x800: fpword |= _RC_UP; break;
        case 0x400: fpword |= _RC_DOWN; break;
    }
    switch (cw & 0x300)
    {
        case 0x0:   fpword |= _PC_24; break;
        case 0x200: fpword |= _PC_53; break;
        case 0x300: fpword |= _PC_64; break;
    }
    if (cw & 0x1000) fpword |= _IC_AFFINE;
    _control87(fpword, _MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC);

    return res;
}
