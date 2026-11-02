/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     Floating-point comparison, classification and complex number helpers from Wine 11.21 dlls/msvcrt/math.c
 * COPYRIGHT:   Copyright 2000 Jon Griffiths
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

int __cdecl _dpcomp(double x, double y)
{
    if (isnan(x) || isnan(y))
        return 0;

    if (x == y) return _FP_EQ;
    return x < y ? _FP_LT : _FP_GT;
}

int __cdecl _fdpcomp(float x, float y)
{
    return _dpcomp(x, y);
}

int __cdecl _fpclassf(float num)
{
    union { float f; uint32_t i; } u = { num };
    int e = u.i >> 23 & 0xff;
    int s = u.i >> 31;

    switch (e)
    {
    case 0:
        if (u.i << 1) return s ? _FPCLASS_ND : _FPCLASS_PD;
        return s ? _FPCLASS_NZ : _FPCLASS_PZ;
    case 0xff:
        if (u.i << 9) return ((u.i >> 22) & 1) ? _FPCLASS_QNAN : _FPCLASS_SNAN;
        return s ? _FPCLASS_NINF : _FPCLASS_PINF;
    default:
        return s ? _FPCLASS_NN : _FPCLASS_PN;
    }
}

_Dcomplex __cdecl _Cbuild(double r, double i)
{
    _Dcomplex ret;
    ret._Val[0] = r;
    ret._Val[1] = i;
    return ret;
}

double __cdecl creal(_Dcomplex z)
{
    return z._Val[0];
}

double __cdecl cimag(_Dcomplex z)
{
    return z._Val[1];
}

_Fcomplex __cdecl _FCbuild(float r, float i)
{
    _Fcomplex ret;
    ret._Val[0] = r;
    ret._Val[1] = i;
    return ret;
}

float __cdecl crealf(_Fcomplex z)
{
    return z._Val[0];
}

float __cdecl cimagf(_Fcomplex z)
{
    return z._Val[1];
}
