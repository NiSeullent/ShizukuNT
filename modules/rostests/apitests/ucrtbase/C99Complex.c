/*
 * PROJECT:     LiberNT API Tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the UCRT complex number construction, access and math exports
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>

#include <windows.h>
#include <float.h>
#include <math.h>

typedef struct { double Value[2]; } DCOMPLEX;
typedef struct { float Value[2]; } FCOMPLEX;

typedef DCOMPLEX (__cdecl *PCBUILD)(double, double);
typedef FCOMPLEX (__cdecl *PFCBUILD)(float, float);
typedef double (__cdecl *PDCOMPLEX_TO_DOUBLE)(DCOMPLEX);
typedef float (__cdecl *PFCOMPLEX_TO_FLOAT)(FCOMPLEX);
typedef DCOMPLEX (__cdecl *PDCOMPLEX_UNARY)(DCOMPLEX);
typedef double (__cdecl *PCABS)(struct _complex);

static FARPROC RequireExport(HMODULE Module, const char *Name)
{
    FARPROC Address = GetProcAddress(Module, Name);

    ok(Address != NULL, "%s is not exported\n", Name);
    return Address;
}

static BOOL IsNear(double Value, double Expected, double Tolerance)
{
    double Difference = Value - Expected;

    return (Difference < 0.0 ? -Difference : Difference) <= Tolerance;
}

static DCOMPLEX MakeDouble(double Real, double Imaginary)
{
    DCOMPLEX Value;

    Value.Value[0] = Real;
    Value.Value[1] = Imaginary;
    return Value;
}

static FCOMPLEX MakeFloat(float Real, float Imaginary)
{
    FCOMPLEX Value;

    Value.Value[0] = Real;
    Value.Value[1] = Imaginary;
    return Value;
}

static void CheckConstruction(HMODULE Module)
{
    PCBUILD Cbuild = (PCBUILD)(PVOID)RequireExport(Module, "_Cbuild");
    PFCBUILD FCbuild = (PFCBUILD)(PVOID)RequireExport(Module, "_FCbuild");
    DCOMPLEX Value;
    FCOMPLEX ValueF;

    if (Cbuild)
    {
        Value = Cbuild(1.0, 2.0);
        ok(Value.Value[0] == 1.0 && Value.Value[1] == 2.0, "_Cbuild(1, 2) returned (%g, %g)\n", Value.Value[0], Value.Value[1]);
        Value = Cbuild(3.0, NAN);
        ok(Value.Value[0] == 3.0 && _isnan(Value.Value[1]), "_Cbuild(3, NaN) returned (%g, %g)\n", Value.Value[0], Value.Value[1]);
        Value = Cbuild(-0.0, HUGE_VAL);
        ok(Value.Value[0] == 0.0 && _copysign(1.0, Value.Value[0]) < 0 && Value.Value[1] > DBL_MAX,
           "_Cbuild(-0, inf) returned (%g, %g)\n", Value.Value[0], Value.Value[1]);
    }

    if (FCbuild)
    {
        ValueF = FCbuild(1.0f, 2.0f);
        ok(ValueF.Value[0] == 1.0f && ValueF.Value[1] == 2.0f, "_FCbuild(1, 2) returned (%g, %g)\n", ValueF.Value[0], ValueF.Value[1]);
        ValueF = FCbuild(NAN, 4.0f);
        ok(_isnan(ValueF.Value[0]) && ValueF.Value[1] == 4.0f, "_FCbuild(NaN, 4) returned (%g, %g)\n", ValueF.Value[0], ValueF.Value[1]);
    }
}

static void CheckAccessors(HMODULE Module)
{
    PDCOMPLEX_TO_DOUBLE Creal = (PDCOMPLEX_TO_DOUBLE)(PVOID)RequireExport(Module, "creal");
    PDCOMPLEX_TO_DOUBLE Cimag = (PDCOMPLEX_TO_DOUBLE)(PVOID)RequireExport(Module, "cimag");
    PFCOMPLEX_TO_FLOAT CrealF = (PFCOMPLEX_TO_FLOAT)(PVOID)RequireExport(Module, "crealf");
    PFCOMPLEX_TO_FLOAT CimagF = (PFCOMPLEX_TO_FLOAT)(PVOID)RequireExport(Module, "cimagf");

    if (Creal) ok(Creal(MakeDouble(1.5, -2.5)) == 1.5, "creal returned %g\n", Creal(MakeDouble(1.5, -2.5)));
    if (Cimag) ok(Cimag(MakeDouble(1.5, -2.5)) == -2.5, "cimag returned %g\n", Cimag(MakeDouble(1.5, -2.5)));
    if (CrealF) ok(CrealF(MakeFloat(1.5f, -2.5f)) == 1.5f, "crealf returned %g\n", CrealF(MakeFloat(1.5f, -2.5f)));
    if (CimagF) ok(CimagF(MakeFloat(1.5f, -2.5f)) == -2.5f, "cimagf returned %g\n", CimagF(MakeFloat(1.5f, -2.5f)));
}

static void CheckArgumentAndExponential(HMODULE Module)
{
    PDCOMPLEX_TO_DOUBLE Carg = (PDCOMPLEX_TO_DOUBLE)(PVOID)RequireExport(Module, "carg");
    PFCOMPLEX_TO_FLOAT CargF = (PFCOMPLEX_TO_FLOAT)(PVOID)RequireExport(Module, "cargf");
    PDCOMPLEX_UNARY Cexp = (PDCOMPLEX_UNARY)(PVOID)RequireExport(Module, "cexp");
    DCOMPLEX Value;

    if (Carg)
    {
        ok(IsNear(Carg(MakeDouble(0.0, 1.0)), 1.5707963267948966, 3e-16), "carg(i) returned %.17g\n", Carg(MakeDouble(0.0, 1.0)));
        ok(IsNear(Carg(MakeDouble(-1.0, 0.0)), 3.1415926535897931, 5e-16), "carg(-1) returned %.17g\n", Carg(MakeDouble(-1.0, 0.0)));
        ok(Carg(MakeDouble(1.0, -0.0)) == 0.0 && _copysign(1.0, Carg(MakeDouble(1.0, -0.0))) < 0,
           "carg(1 - 0i) returned %.17g\n", Carg(MakeDouble(1.0, -0.0)));
    }
    if (CargF)
        ok(IsNear(CargF(MakeFloat(1.0f, 1.0f)), 0.78539816f, 1e-7), "cargf(1 + i) returned %.9g\n", CargF(MakeFloat(1.0f, 1.0f)));

    if (Cexp)
    {
        Value = Cexp(MakeDouble(0.0, 0.0));
        ok(Value.Value[0] == 1.0 && Value.Value[1] == 0.0, "cexp(0) returned (%.17g, %.17g)\n", Value.Value[0], Value.Value[1]);
        Value = Cexp(MakeDouble(1.0, 0.0));
        ok(IsNear(Value.Value[0], 2.7182818284590451, 5e-16) && Value.Value[1] == 0.0,
           "cexp(1) returned (%.17g, %.17g)\n", Value.Value[0], Value.Value[1]);
        Value = Cexp(MakeDouble(0.0, 2.0));
        ok(IsNear(Value.Value[0], -0.41614683654714241, 2e-16) && IsNear(Value.Value[1], 0.90929742682568171, 2e-16),
           "cexp(2i) returned (%.17g, %.17g)\n", Value.Value[0], Value.Value[1]);
    }
}

static void CheckCabs(HMODULE Module)
{
    static const char *const Names[] = {"_cabs", "_o__cabs"};
    struct _complex Value = {3.0, 4.0};
    PCABS Cabs;
    SIZE_T Name;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Cabs = (PCABS)(PVOID)RequireExport(Module, Names[Name]);
        if (Cabs) ok(Cabs(Value) == 5.0, "%s(3 + 4i) returned %.17g\n", Names[Name], Cabs(Value));
    }
}

START_TEST(C99Complex)
{
    HMODULE Module = GetModuleHandleW(L"ucrtbase.dll");

    ok(Module != NULL, "ucrtbase.dll is not loaded\n");
    if (!Module) return;

    CheckConstruction(Module);
    CheckAccessors(Module);
    CheckArgumentAndExponential(Module);
    CheckCabs(Module);
}
