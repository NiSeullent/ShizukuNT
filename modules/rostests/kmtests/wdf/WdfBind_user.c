/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     KMDF bind, version and timer test user-mode part
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <kmt_test.h>

#include "WdfBind.h"

#define WDFBIND_WDF_KEY L"SYSTEM\\CurrentControlSet\\Services\\Kmtest-WdfBind\\Parameters\\Wdf"

static
DWORD
QueryDword(
    _In_ HKEY Key,
    _In_ PCWSTR Name,
    _Out_ PDWORD Value)
{
    DWORD Type;
    DWORD Size = sizeof(*Value);
    DWORD Error;

    *Value = 0;
    Error = RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)Value, &Size);
    if (Error == ERROR_SUCCESS && (Type != REG_DWORD || Size != sizeof(*Value)))
        Error = ERROR_INVALID_DATA;
    return Error;
}

static
VOID
QueryFrameworkVersion(
    _Out_ PWDFBIND_PARAMS Params)
{
    DWORD Major, Minor;
    HKEY Key;
    DWORD Error;

    Params->MajorVersion = 0;
    Params->MinorVersion = 0;

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Services\\Wdf01000\\Parameters",
                          0,
                          KEY_QUERY_VALUE,
                          &Key);
    if (skip(Error == ERROR_SUCCESS, "Wdf01000 parameters not found: %lu\n", Error))
        return;

    if (QueryDword(Key, L"MajorVersion", &Major) == ERROR_SUCCESS &&
        QueryDword(Key, L"MinorVersion", &Minor) == ERROR_SUCCESS)
    {
        Params->MajorVersion = Major;
        Params->MinorVersion = Minor;
    }

    RegCloseKey(Key);
}

static
VOID
TestLibraryRegistration(
    _In_ PWDFBIND_PARAMS Params)
{
    WCHAR KeyName[MAX_PATH];
    WCHAR Expected[32];
    WCHAR Value[MAX_PATH];
    DWORD Size;
    DWORD Error;

    if (skip(Params->MajorVersion != 0, "The framework version is not registered\n"))
        return;

    StringCbPrintfW(KeyName, sizeof(KeyName),
                    L"SYSTEM\\CurrentControlSet\\Control\\Wdf\\Kmdf\\KmdfLibrary\\Versions\\%lu",
                    Params->MajorVersion);
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, KeyName, L"Service", RRF_RT_REG_SZ, NULL, Value, &Size);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    if (Error == ERROR_SUCCESS)
        ok(!_wcsicmp(Value, L"Wdf01000"), "Service = %ls, expected Wdf01000\n", Value);

    StringCbPrintfW(KeyName, sizeof(KeyName),
                    L"SYSTEM\\CurrentControlSet\\Control\\Wdf\\Kmdf\\%lu",
                    Params->MajorVersion);
    Size = sizeof(Value);
    Error = RegGetValueW(HKEY_LOCAL_MACHINE, KeyName, L"Version", RRF_RT_REG_SZ, NULL, Value, &Size);
    if (!skip(Error == ERROR_SUCCESS, "No KMDF version string: %lu\n", Error))
    {
        StringCbPrintfW(Expected, sizeof(Expected), L"%lu.%lu",
                        Params->MajorVersion, Params->MinorVersion);
        ok_eq_wstr(Value, Expected);
    }
}

static
DWORD
SetEnhancedVerifierOptions(
    _In_ DWORD Options)
{
    HKEY Key;
    DWORD Error;

    if (Options == 0)
    {
        Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, WDFBIND_WDF_KEY, 0, KEY_SET_VALUE, &Key);
        if (Error == ERROR_FILE_NOT_FOUND)
            return ERROR_SUCCESS;
        if (Error != ERROR_SUCCESS)
            return Error;

        Error = RegDeleteValueW(Key, L"EnhancedVerifierOptions");
        if (Error == ERROR_FILE_NOT_FOUND)
            Error = ERROR_SUCCESS;
    }
    else
    {
        Error = RegCreateKeyExW(HKEY_LOCAL_MACHINE, WDFBIND_WDF_KEY, 0, NULL,
                                REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &Key, NULL);
        if (Error != ERROR_SUCCESS)
            return Error;

        Error = RegSetValueExW(Key, L"EnhancedVerifierOptions", 0, REG_DWORD,
                               (const BYTE *)&Options, sizeof(Options));
    }

    RegCloseKey(Key);
    return Error;
}

static
ULONG64
RunWdfBind(
    _In_ PWDFBIND_PARAMS Params,
    _In_ BOOLEAN DeleteService)
{
    WDFBIND_PARAMS Buffer = *Params;
    DWORD Length = sizeof(Buffer);
    DWORD Error;

    Buffer.FunctionTable = 0;
    Error = KmtLoadAndOpenDriver(L"WdfBind", FALSE);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    if (Error == ERROR_SUCCESS)
    {
        Error = KmtSendBufferToDriver(IOCTL_WDFBIND_RUN, &Buffer, sizeof(Buffer), &Length);
        ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
        ok_eq_ulong(Length, (DWORD)sizeof(Buffer));
        if (Error != ERROR_SUCCESS || Length != sizeof(Buffer))
            Buffer.FunctionTable = 0;
        KmtCloseDriver();
    }

    if (DeleteService)
        KmtUnloadDriver();
    else
        KmtUnloadDriverKeepService();

    return Buffer.FunctionTable;
}

START_TEST(WdfBind)
{
    WDFBIND_PARAMS Params;
    ULONG64 Table, VerifierTable, TableAgain;

    QueryFrameworkVersion(&Params);
    TestLibraryRegistration(&Params);

    ok_eq_ulong(SetEnhancedVerifierOptions(0), (DWORD)ERROR_SUCCESS);
    Table = RunWdfBind(&Params, FALSE);

    ok_eq_ulong(SetEnhancedVerifierOptions(1), (DWORD)ERROR_SUCCESS);
    VerifierTable = RunWdfBind(&Params, FALSE);

    ok_eq_ulong(SetEnhancedVerifierOptions(0), (DWORD)ERROR_SUCCESS);
    TableAgain = RunWdfBind(&Params, TRUE);

    ok(Table != 0, "No function table without the enhanced verifier\n");
    ok(VerifierTable != 0, "No function table with the enhanced verifier\n");
    ok(TableAgain == Table, "Function table 0x%I64x, expected 0x%I64x\n", TableAgain, Table);
    ok(VerifierTable != Table, "The enhanced verifier returned the regular function table 0x%I64x\n", Table);
}
