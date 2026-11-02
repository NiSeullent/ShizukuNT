/*
 * PROJECT:     LiberNT API Tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for mainstream UCRT C99 math exports
 * COPYRIGHT:   Copyright 2026 Ahmed Arif <arif.ing@outlook.com>
 */

#include <apitest.h>

#include <windows.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>

typedef double (__cdecl *PUNARY_DOUBLE)(double);
typedef float (__cdecl *PUNARY_FLOAT)(float);
typedef double (__cdecl *PBINARY_DOUBLE)(double, double);
typedef float (__cdecl *PBINARY_FLOAT)(float, float);
typedef void (__cdecl *PSET_USER_MATHERR)(int (__cdecl *)(struct _exception *));
typedef int (__cdecl *PILOGB_DOUBLE)(double);
typedef int (__cdecl *PILOGB_FLOAT)(float);
typedef double (__cdecl *PNAN_DOUBLE)(const char *);
typedef float (__cdecl *PNAN_FLOAT)(const char *);
typedef double (__cdecl *PREMQUO_DOUBLE)(double, double, int *);
typedef float (__cdecl *PREMQUO_FLOAT)(float, float, int *);
typedef int (__cdecl *PCOMPARE_DOUBLE)(double, double);
typedef int (__cdecl *PCOMPARE_FLOAT)(float, float);
typedef int (__cdecl *PCLASSIFY_FLOAT)(float);
typedef int (__cdecl *PINT_VOID)(void);
typedef int (__cdecl *PINT_INT)(int);

static struct _exception LastException;
static int MatherrCalls;
static int MatherrReturn;
static double MatherrResult;

static int __cdecl MatherrCallback(struct _exception *Exception)
{
    LastException = *Exception;
    MatherrCalls++;
    if (MatherrReturn) Exception->retval = MatherrResult;
    return MatherrReturn;
}

static FARPROC RequireExport(HMODULE Module, const char *Name)
{
    FARPROC Address = GetProcAddress(Module, Name);

    ok(Address != NULL, "%s is not exported\n", Name);
    return Address;
}

static double AbsoluteDifference(double Left, double Right)
{
    double Difference = Left - Right;

    return Difference < 0.0 ? -Difference : Difference;
}

static void CheckDouble(HMODULE Module, const char *Name, double Input, double Expected, double Tolerance)
{
    PUNARY_DOUBLE Function = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, Name);
    double Result;

    if (!Function) return;
    Result = Function(Input);
    ok(AbsoluteDifference(Result, Expected) <= Tolerance, "%s(%.17g) returned %.17g, expected %.17g\n", Name, Input, Result, Expected);
}

static void CheckFloat(HMODULE Module, const char *Name, float Input, float Expected, float Tolerance)
{
    PUNARY_FLOAT Function = (PUNARY_FLOAT)(PVOID)RequireExport(Module, Name);
    float Result;

    if (!Function) return;
    Result = Function(Input);
    ok(AbsoluteDifference(Result, Expected) <= Tolerance, "%s(%.9g) returned %.9g, expected %.9g\n", Name, Input, Result, Expected);
}

static void CheckDoubleAliases(HMODULE Module, const char *const *Names, SIZE_T Count, double Input, double Expected, double Tolerance)
{
    SIZE_T Index;

    for (Index = 0; Index < Count; Index++) CheckDouble(Module, Names[Index], Input, Expected, Tolerance);
}

static void CheckDomainErrors(HMODULE Module)
{
    PUNARY_DOUBLE Acosh = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, "acosh");
    PUNARY_DOUBLE Atanh = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, "atanh");
    PUNARY_DOUBLE Log1p = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, "log1p");
    double Result;

    if (Acosh)
    {
        errno = -1;
        Result = Acosh(0.0);
        ok(Result != Result, "acosh(0) returned %.17g instead of NaN\n", Result);
        ok_long(errno, EDOM);
    }

    if (Atanh)
    {
        errno = -1;
        Result = Atanh(1.0);
        ok(Result > DBL_MAX, "atanh(1) returned %.17g instead of infinity\n", Result);
        ok_long(errno, ERANGE);

        errno = -1;
        Result = Atanh(2.0);
        ok(Result != Result, "atanh(2) returned %.17g instead of NaN\n", Result);
        ok_long(errno, EDOM);
    }

    if (Log1p)
    {
        errno = -1;
        Result = Log1p(-1.0);
        ok(Result < -DBL_MAX, "log1p(-1) returned %.17g instead of negative infinity\n", Result);
        ok_long(errno, ERANGE);

        errno = -1;
        Result = Log1p(-2.0);
        ok(Result != Result, "log1p(-2) returned %.17g instead of NaN\n", Result);
        ok_long(errno, EDOM);
    }
}

static void CheckMatherrIsolation(HMODULE Module)
{
    PSET_USER_MATHERR SetUserMatherr = (PSET_USER_MATHERR)(PVOID)RequireExport(Module, "__setusermatherr");
    PUNARY_DOUBLE Expm1 = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, "expm1");
    double Result;

    if (!SetUserMatherr || !Expm1) return;
    ZeroMemory(&LastException, sizeof(LastException));
    MatherrCalls = 0;
    MatherrReturn = 0;
    SetUserMatherr(MatherrCallback);
    errno = -1;
    Result = Expm1(1000.0);
    ok(Result > DBL_MAX, "expm1(1000) returned %.17g instead of infinity\n", Result);
    ok_long(errno, ERANGE);
    ok_long(MatherrCalls, 0);

    MatherrCalls = 0;
    MatherrReturn = 1;
    MatherrResult = 42.0;
    errno = -1;
    Result = Expm1(1000.0);
    ok(Result > DBL_MAX, "expm1 invoked the legacy matherr replacement and returned %.17g\n", Result);
    ok_long(errno, ERANGE);
    ok_long(MatherrCalls, 0);
    SetUserMatherr(NULL);
}

static double DoubleFromBits(ULONGLONG Bits)
{
    union { ULONGLONG Bits; double Value; } u;

    u.Bits = Bits;
    return u.Value;
}

static float FloatFromBits(ULONG Bits)
{
    union { ULONG Bits; float Value; } u;

    u.Bits = Bits;
    return u.Value;
}

static void CheckIlogb(HMODULE Module)
{
    static const char *const Names[] = {"ilogb", "ilogbl"};
    static const struct { double x; int Expected; } Tests[] =
    {
        {1.0, 0}, {1024.0, 10}, {0.75, -1}, {-8.0, 3}, {DBL_MIN, -1022},
        {0.0, FP_ILOGB0}, {-0.0, FP_ILOGB0}, {HUGE_VAL, INT_MAX}, {-HUGE_VAL, INT_MAX},
    };
    static const struct { float x; int Expected; } TestsF[] =
    {
        {8.0f, 3}, {0.75f, -1}, {FLT_MIN, -126}, {0.0f, FP_ILOGB0}, {HUGE_VALF, INT_MAX},
    };
    PILOGB_DOUBLE Ilogb;
    PILOGB_FLOAT IlogbF;
    SIZE_T Index, Name;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Ilogb = (PILOGB_DOUBLE)(PVOID)RequireExport(Module, Names[Name]);
        if (!Ilogb) continue;
        for (Index = 0; Index < _countof(Tests); Index++)
            ok(Ilogb(Tests[Index].x) == Tests[Index].Expected, "%s(%.17g) returned %d, expected %d\n",
               Names[Name], Tests[Index].x, Ilogb(Tests[Index].x), Tests[Index].Expected);
        ok(Ilogb(DoubleFromBits(1)) == -1074, "%s(DBL_TRUE_MIN) returned %d\n", Names[Name], Ilogb(DoubleFromBits(1)));
        ok(Ilogb(NAN) == FP_ILOGBNAN, "%s(NaN) returned %d\n", Names[Name], Ilogb(NAN));
    }

    IlogbF = (PILOGB_FLOAT)(PVOID)RequireExport(Module, "ilogbf");
    if (!IlogbF) return;
    for (Index = 0; Index < _countof(TestsF); Index++)
        ok(IlogbF(TestsF[Index].x) == TestsF[Index].Expected, "ilogbf(%.9g) returned %d, expected %d\n",
           TestsF[Index].x, IlogbF(TestsF[Index].x), TestsF[Index].Expected);
    ok(IlogbF(FloatFromBits(1)) == -149, "ilogbf(FLT_TRUE_MIN) returned %d\n", IlogbF(FloatFromBits(1)));
    ok(IlogbF(NAN) == FP_ILOGBNAN, "ilogbf(NaN) returned %d\n", IlogbF(NAN));
}

static void CheckNan(HMODULE Module)
{
    static const char *const Names[] = {"nan", "nanl", "_o_nan", "_o_nanl"};
    static const char *const NamesF[] = {"nanf", "_o_nanf"};
    PNAN_DOUBLE Nan;
    PNAN_FLOAT NanF;
    SIZE_T Name;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Nan = (PNAN_DOUBLE)(PVOID)RequireExport(Module, Names[Name]);
        if (!Nan) continue;
        ok(_fpclass(Nan("")) == _FPCLASS_QNAN, "%s(\"\") returned class %#x\n", Names[Name], _fpclass(Nan("")));
    }
    for (Name = 0; Name < _countof(NamesF); Name++)
    {
        NanF = (PNAN_FLOAT)(PVOID)RequireExport(Module, NamesF[Name]);
        if (!NanF) continue;
        ok(_fpclass(NanF("")) == _FPCLASS_QNAN, "%s(\"\") returned class %#x\n", NamesF[Name], _fpclass(NanF("")));
    }
}

static BOOL QuotientMatches(int Quotient, int Expected)
{
    if ((Quotient < 0) != (Expected < 0)) return FALSE;
    return ((Quotient < 0 ? -Quotient : Quotient) & 7) == ((Expected < 0 ? -Expected : Expected) & 7);
}

static void CheckRemquo(HMODULE Module)
{
    static const char *const Names[] = {"remquo", "remquol", "_o_remquo", "_o_remquol"};
    static const char *const NamesF[] = {"remquof", "_o_remquof"};
    static const struct { double x, y, Expected; int Quotient; } Tests[] =
    {
        {10.0, 3.0, 1.0, 3}, {-10.0, 3.0, -1.0, -3}, {10.0, -3.0, 1.0, -3},
        {7.0, 2.0, -1.0, 4}, {5.0, 2.0, 1.0, 2},
    };
    PREMQUO_DOUBLE Remquo;
    PREMQUO_FLOAT RemquoF;
    SIZE_T Index, Name;
    double Result;
    float ResultF;
    int Quotient;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Remquo = (PREMQUO_DOUBLE)(PVOID)RequireExport(Module, Names[Name]);
        if (!Remquo) continue;
        for (Index = 0; Index < _countof(Tests); Index++)
        {
            Quotient = 0x55;
            Result = Remquo(Tests[Index].x, Tests[Index].y, &Quotient);
            ok(Result == Tests[Index].Expected, "%s(%g, %g) returned %.17g, expected %g\n",
               Names[Name], Tests[Index].x, Tests[Index].y, Result, Tests[Index].Expected);
            ok(QuotientMatches(Quotient, Tests[Index].Quotient), "%s(%g, %g) stored quotient %d, expected %d\n",
               Names[Name], Tests[Index].x, Tests[Index].y, Quotient, Tests[Index].Quotient);
        }
        Result = Remquo(1.0, 0.0, &Quotient);
        ok(_isnan(Result), "%s(1, 0) returned %.17g instead of NaN\n", Names[Name], Result);
        Result = Remquo(HUGE_VAL, 1.0, &Quotient);
        ok(_isnan(Result), "%s(inf, 1) returned %.17g instead of NaN\n", Names[Name], Result);
    }

    for (Name = 0; Name < _countof(NamesF); Name++)
    {
        RemquoF = (PREMQUO_FLOAT)(PVOID)RequireExport(Module, NamesF[Name]);
        if (!RemquoF) continue;
        Quotient = 0x55;
        ResultF = RemquoF(10.0f, 3.0f, &Quotient);
        ok(ResultF == 1.0f && QuotientMatches(Quotient, 3), "%s(10, 3) returned %.9g, quotient %d\n", NamesF[Name], ResultF, Quotient);
        Quotient = 0x55;
        ResultF = RemquoF(7.0f, 2.0f, &Quotient);
        ok(ResultF == -1.0f && QuotientMatches(Quotient, 4), "%s(7, 2) returned %.9g, quotient %d\n", NamesF[Name], ResultF, Quotient);
        ResultF = RemquoF(1.0f, 0.0f, &Quotient);
        ok(_isnan(ResultF), "%s(1, 0) returned %.9g instead of NaN\n", NamesF[Name], ResultF);
    }
}

static void CheckGamma(HMODULE Module)
{
    static const char *const TgammaNames[] = {"tgamma", "tgammal", "_o_tgamma", "_o_tgammal"};
    static const char *const LgammaNames[] = {"lgamma", "lgammal", "_o_lgamma", "_o_lgammal"};
    PUNARY_DOUBLE Function;
    SIZE_T Name;
    double Result;

    for (Name = 0; Name < _countof(TgammaNames); Name++)
    {
        CheckDouble(Module, TgammaNames[Name], 5.0, 24.0, 1e-14);
        CheckDouble(Module, TgammaNames[Name], 0.5, 1.7724538509055160, 2e-15);
        CheckDouble(Module, TgammaNames[Name], -0.5, -3.5449077018110321, 4e-15);
        Function = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, TgammaNames[Name]);
        if (!Function) continue;
        Result = Function(0.0);
        ok(Result > DBL_MAX, "%s(+0) returned %.17g instead of +infinity\n", TgammaNames[Name], Result);
        Result = Function(-0.0);
        ok(Result < -DBL_MAX, "%s(-0) returned %.17g instead of -infinity\n", TgammaNames[Name], Result);
        Result = Function(-1.0);
        ok(_isnan(Result), "%s(-1) returned %.17g instead of NaN\n", TgammaNames[Name], Result);
        Result = Function(-HUGE_VAL);
        ok(_isnan(Result), "%s(-inf) returned %.17g instead of NaN\n", TgammaNames[Name], Result);
        Result = Function(HUGE_VAL);
        ok(Result > DBL_MAX, "%s(+inf) returned %.17g instead of +infinity\n", TgammaNames[Name], Result);
        Result = Function(200.0);
        ok(Result > DBL_MAX, "%s(200) returned %.17g instead of +infinity\n", TgammaNames[Name], Result);
    }
    CheckFloat(Module, "tgammaf", 5.0f, 24.0f, 2e-5f);
    CheckFloat(Module, "_o_tgammaf", 0.5f, 1.77245385f, 2e-6f);

    for (Name = 0; Name < _countof(LgammaNames); Name++)
    {
        CheckDouble(Module, LgammaNames[Name], 1.0, 0.0, 0.0);
        CheckDouble(Module, LgammaNames[Name], 2.0, 0.0, 0.0);
        CheckDouble(Module, LgammaNames[Name], 0.5, 0.57236494292470008, 2e-15);
        CheckDouble(Module, LgammaNames[Name], -0.5, 1.2655121234846454, 4e-15);
        CheckDouble(Module, LgammaNames[Name], 10.0, 12.801827480081469, 4e-14);
        Function = (PUNARY_DOUBLE)(PVOID)RequireExport(Module, LgammaNames[Name]);
        if (!Function) continue;
        Result = Function(0.0);
        ok(Result > DBL_MAX, "%s(0) returned %.17g instead of +infinity\n", LgammaNames[Name], Result);
        Result = Function(-1.0);
        ok(Result > DBL_MAX, "%s(-1) returned %.17g instead of +infinity\n", LgammaNames[Name], Result);
        Result = Function(-HUGE_VAL);
        ok(Result > DBL_MAX, "%s(-inf) returned %.17g instead of +infinity\n", LgammaNames[Name], Result);
        Result = Function(HUGE_VAL);
        ok(Result > DBL_MAX, "%s(+inf) returned %.17g instead of +infinity\n", LgammaNames[Name], Result);
    }
    CheckFloat(Module, "lgammaf", 1.0f, 0.0f, 0.0f);
    CheckFloat(Module, "_o_lgammaf", 0.5f, 0.572364943f, 2e-7f);
}

static void CheckComparison(HMODULE Module)
{
    static const char *const Names[] = {"_dpcomp", "_ldpcomp", "_o__dpcomp", "_o__ldpcomp"};
    static const char *const NamesF[] = {"_fdpcomp", "_o__fdpcomp"};
    static const struct { double x, y; int Expected; } Tests[] =
    {
        {0.0, 0.0, _FP_EQ}, {1.0, 1.0, _FP_EQ}, {-1.0, -1.0, _FP_EQ}, {0.0, -0.0, _FP_EQ},
        {-2.0, -1.0, _FP_LT}, {-1.0, 1.0, _FP_LT}, {1.0, 2.0, _FP_LT},
        {1.0, -1.0, _FP_GT}, {2.0, 1.0, _FP_GT}, {-1.0, -2.0, _FP_GT},
        {HUGE_VAL, HUGE_VAL, _FP_EQ}, {-1.0, HUGE_VAL, _FP_LT}, {-HUGE_VAL, 1.0, _FP_LT},
        {NAN, NAN, 0}, {NAN, 1.0, 0}, {1.0, NAN, 0},
    };
    PCOMPARE_DOUBLE Compare;
    PCOMPARE_FLOAT CompareF;
    SIZE_T Index, Name;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Compare = (PCOMPARE_DOUBLE)(PVOID)RequireExport(Module, Names[Name]);
        if (!Compare) continue;
        for (Index = 0; Index < _countof(Tests); Index++)
            ok(Compare(Tests[Index].x, Tests[Index].y) == Tests[Index].Expected, "%s(%g, %g) returned %d, expected %d\n",
               Names[Name], Tests[Index].x, Tests[Index].y, Compare(Tests[Index].x, Tests[Index].y), Tests[Index].Expected);
    }
    for (Name = 0; Name < _countof(NamesF); Name++)
    {
        CompareF = (PCOMPARE_FLOAT)(PVOID)RequireExport(Module, NamesF[Name]);
        if (!CompareF) continue;
        for (Index = 0; Index < _countof(Tests); Index++)
            ok(CompareF((float)Tests[Index].x, (float)Tests[Index].y) == Tests[Index].Expected, "%s(%g, %g) returned %d, expected %d\n",
               NamesF[Name], Tests[Index].x, Tests[Index].y, CompareF((float)Tests[Index].x, (float)Tests[Index].y), Tests[Index].Expected);
    }

    ok(isless(1.0, 2.0), "isless(1, 2) is false\n");
    ok(!isless(2.0, 1.0), "isless(2, 1) is true\n");
    ok(!isless(NAN, 1.0), "isless(NaN, 1) is true\n");
    ok(isgreater(2.0f, 1.0f), "isgreater(2f, 1f) is false\n");
    ok(islessequal(1.0, 1.0), "islessequal(1, 1) is false\n");
    ok(isgreaterequal(-HUGE_VAL, -HUGE_VAL), "isgreaterequal(-inf, -inf) is false\n");
    ok(islessgreater(1.0, 2.0), "islessgreater(1, 2) is false\n");
    ok(!islessgreater(NAN, 1.0), "islessgreater(NaN, 1) is true\n");
    ok(isunordered(NAN, 1.0), "isunordered(NaN, 1) is false\n");
    ok(!isunordered(1.0, 2.0), "isunordered(1, 2) is true\n");
}

static void CheckFpclassf(HMODULE Module)
{
    static const char *const Names[] = {"_fpclassf", "_o__fpclassf"};
#if defined(_M_IX86)
    SIZE_T Name;

    for (Name = 0; Name < _countof(Names); Name++)
        ok(GetProcAddress(Module, Names[Name]) == NULL, "%s is exported on x86\n", Names[Name]);
#else
    static const struct { ULONG Bits; int Expected; } Tests[] =
    {
        {0x3f800000, _FPCLASS_PN}, {0xbf800000, _FPCLASS_NN}, {0x00000000, _FPCLASS_PZ}, {0x80000000, _FPCLASS_NZ},
        {0x00000001, _FPCLASS_PD}, {0x80000001, _FPCLASS_ND}, {0x7f800000, _FPCLASS_PINF}, {0xff800000, _FPCLASS_NINF},
        {0x7fc00000, _FPCLASS_QNAN}, {0xffc00000, _FPCLASS_QNAN}, {0x7f800001, _FPCLASS_SNAN},
    };
    PCLASSIFY_FLOAT Classify;
    SIZE_T Index, Name;

    for (Name = 0; Name < _countof(Names); Name++)
    {
        Classify = (PCLASSIFY_FLOAT)(PVOID)RequireExport(Module, Names[Name]);
        if (!Classify) continue;
        for (Index = 0; Index < _countof(Tests); Index++)
            ok(Classify(FloatFromBits(Tests[Index].Bits)) == Tests[Index].Expected, "%s(%#lx) returned %#x, expected %#x\n",
               Names[Name], Tests[Index].Bits, Classify(FloatFromBits(Tests[Index].Bits)), Tests[Index].Expected);
    }
#endif
}

static void CheckLibrarySwitches(HMODULE Module)
{
#if defined(_M_AMD64) && !defined(_M_ARM64EC)
    PINT_INT SetFma3 = (PINT_INT)(PVOID)RequireExport(Module, "_set_FMA3_enable");
    PINT_VOID GetFma3 = (PINT_VOID)(PVOID)RequireExport(Module, "_get_FMA3_enable");
    int Previous, Enabled;

    if (!SetFma3 || !GetFma3) return;
    Previous = GetFma3();
    ok(SetFma3(0) == 0, "_set_FMA3_enable(0) did not disable the FMA3 library\n");
    ok(GetFma3() == 0, "_get_FMA3_enable returned %d after disabling\n", GetFma3());
    Enabled = SetFma3(1);
    ok(GetFma3() == Enabled, "_get_FMA3_enable returned %d, _set_FMA3_enable(1) returned %d\n", GetFma3(), Enabled);
    SetFma3(Previous);
    ok(GetFma3() == (Previous ? Enabled : 0), "_get_FMA3_enable returned %d after restoring %d\n", GetFma3(), Previous);
#elif defined(_M_IX86)
    PINT_INT SetSse2 = (PINT_INT)(PVOID)RequireExport(Module, "_set_SSE2_enable");
    BOOL Supported = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE);

    if (!SetSse2) return;
    ok(SetSse2(0) == 0, "_set_SSE2_enable(0) did not disable the SSE2 implementation\n");
    ok(!SetSse2(1) == !Supported, "_set_SSE2_enable(1) does not match SSE2 support %d\n", Supported);
#else
    (void)Module;
    skip("FMA3 and SSE2 library switches are x86 and x64 only\n");
#endif
}

START_TEST(C99Math)
{
    static const char *const AcoshNames[] = {"acosh", "acoshl"};
    static const char *const AsinhNames[] = {"asinh", "asinhl"};
    static const char *const AtanhNames[] = {"atanh", "atanhl"};
    static const char *const CbrtNames[] = {"cbrt", "cbrtl"};
    static const char *const ErfNames[] = {"erf", "erfl"};
    static const char *const ErfcNames[] = {"erfc", "erfcl"};
    static const char *const Exp2Names[] = {"exp2", "exp2l"};
    static const char *const Expm1Names[] = {"expm1", "expm1l"};
    static const char *const Log1pNames[] = {"log1p", "log1pl"};
    PBINARY_DOUBLE CopySign;
    PBINARY_DOUBLE CopySignLong;
    PBINARY_FLOAT CopySignFloat;
    HMODULE Module;

    Module = GetModuleHandleW(L"ucrtbase.dll");
    ok(Module != NULL, "ucrtbase.dll is not loaded\n");
    if (!Module) return;

    CheckDoubleAliases(Module, AcoshNames, _countof(AcoshNames), 2.0, 1.3169578969248166, 2e-15);
    CheckFloat(Module, "acoshf", 2.0f, 1.31695795f, 2e-7f);
    CheckDoubleAliases(Module, AsinhNames, _countof(AsinhNames), 0.5, 0.48121182505960347, 2e-15);
    CheckFloat(Module, "asinhf", 0.5f, 0.481211811f, 2e-7f);
    CheckDoubleAliases(Module, AtanhNames, _countof(AtanhNames), 0.5, 0.54930614433405489, 2e-15);
    CheckFloat(Module, "atanhf", 0.5f, 0.549306154f, 2e-7f);
    CheckDoubleAliases(Module, CbrtNames, _countof(CbrtNames), -8.0, -2.0, 0.0);
    CheckFloat(Module, "cbrtf", -8.0f, -2.0f, 0.0f);
    CheckDoubleAliases(Module, ErfNames, _countof(ErfNames), 1.0, 0.84270079294971489, 2e-15);
    CheckFloat(Module, "erff", 1.0f, 0.842700779f, 2e-7f);
    CheckDoubleAliases(Module, ErfcNames, _countof(ErfcNames), 1.0, 0.15729920705028513, 2e-15);
    CheckFloat(Module, "erfcf", 1.0f, 0.157299206f, 2e-7f);
    CheckDoubleAliases(Module, Exp2Names, _countof(Exp2Names), -15.0, 3.0517578125e-5, 0.0);
    CheckFloat(Module, "exp2f", -15.0f, 3.0517578125e-5f, 0.0f);
    CheckDoubleAliases(Module, Expm1Names, _countof(Expm1Names), 1e-8, 1.0000000050000001e-8, 2e-24);
    CheckFloat(Module, "expm1f", 1e-4f, 1.00004996e-4f, 2e-11f);
    CheckDoubleAliases(Module, Log1pNames, _countof(Log1pNames), 1e-16, 1e-16, 1e-31);
    CheckFloat(Module, "log1pf", 1e-4f, 9.99949989e-5f, 2e-11f);

    CopySign = (PBINARY_DOUBLE)(PVOID)RequireExport(Module, "copysign");
    CopySignLong = (PBINARY_DOUBLE)(PVOID)RequireExport(Module, "copysignl");
    CopySignFloat = (PBINARY_FLOAT)(PVOID)RequireExport(Module, "copysignf");
    if (CopySign) ok(CopySign(3.0, -0.0) == -3.0, "copysign did not copy the sign\n");
    if (CopySignLong) ok(CopySignLong(3.0, -0.0) == -3.0, "copysignl did not copy the sign\n");
    if (CopySignFloat) ok(CopySignFloat(3.0f, -0.0f) == -3.0f, "copysignf did not copy the sign\n");

    CheckDomainErrors(Module);
    CheckMatherrIsolation(Module);
    CheckIlogb(Module);
    CheckNan(Module);
    CheckRemquo(Module);
    CheckGamma(Module);
    CheckComparison(Module);
    CheckFpclassf(Module);
    CheckLibrarySwitches(Module);
}
