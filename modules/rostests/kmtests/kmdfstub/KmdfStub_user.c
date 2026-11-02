/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Permissive KMDF client stub test user-mode part
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <kmt_test.h>

#include "KmdfStub.h"

#define KMDFSTUB_SERVICE_NAME L"Kmtest-KmdfStub"
#define KMDFSTUB_SERVICE_KEY L"SYSTEM\\CurrentControlSet\\Services\\" KMDFSTUB_SERVICE_NAME
#define KMDFSTUB_NOUNLOAD_SERVICE_NAME L"Kmtest-KmdfStubNoUnload"
#define KMDFSTUB_NOUNLOAD_SERVICE_KEY L"SYSTEM\\CurrentControlSet\\Services\\" KMDFSTUB_NOUNLOAD_SERVICE_NAME

static
DWORD
DeleteCleanupValue(
    _In_ PCWSTR ServiceKey)
{
    DWORD Error;

    Error = RegDeleteKeyValueW(HKEY_LOCAL_MACHINE, ServiceKey, KMDFSTUB_CLEANUP_VALUE);
    if (Error == ERROR_FILE_NOT_FOUND)
        Error = ERROR_SUCCESS;
    return Error;
}

static
DWORD
QueryCleanupValue(
    _In_ PCWSTR ServiceKey,
    _Out_ PDWORD Value)
{
    DWORD Size = sizeof(*Value);

    *Value = 0;
    return RegGetValueW(HKEY_LOCAL_MACHINE,
                        ServiceKey,
                        KMDFSTUB_CLEANUP_VALUE,
                        RRF_RT_REG_DWORD,
                        NULL,
                        Value,
                        &Size);
}

static
VOID
DeleteTestService(
    _In_ PCWSTR ServiceName)
{
    SC_HANDLE Manager;
    SC_HANDLE Service;

    Manager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    ok(Manager != NULL, "OpenSCManagerW failed: %lu\n", GetLastError());
    if (Manager == NULL)
        return;

    Service = OpenServiceW(Manager, ServiceName, DELETE);
    ok(Service != NULL, "OpenServiceW failed: %lu\n", GetLastError());
    if (Service != NULL)
    {
        ok(DeleteService(Service), "DeleteService failed: %lu\n", GetLastError());
        CloseServiceHandle(Service);
    }

    CloseServiceHandle(Manager);
}

static
VOID
RunKmdfStub(VOID)
{
    DWORD Value;
    DWORD Error;

    ok_eq_ulong(DeleteCleanupValue(KMDFSTUB_SERVICE_KEY), (DWORD)ERROR_SUCCESS);

    Error = KmtLoadAndOpenDriver(L"KmdfStub", FALSE);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    if (Error != ERROR_SUCCESS)
        return;

    Error = KmtSendToDriver(IOCTL_KMDFSTUB_CHECK);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    KmtCloseDriver();
    KmtUnloadDriverKeepService();

    Error = QueryCleanupValue(KMDFSTUB_SERVICE_KEY, &Value);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    ok_eq_ulong(Value, 1UL);
}

static
VOID
RunKmdfStubNoUnload(VOID)
{
    DWORD Value;
    DWORD Error;

    ok_eq_ulong(DeleteCleanupValue(KMDFSTUB_NOUNLOAD_SERVICE_KEY), (DWORD)ERROR_SUCCESS);

    Error = KmtLoadDriver(L"KmdfStubNoUnload", FALSE);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    if (Error != ERROR_SUCCESS)
        return;

    KmtUnloadDriverKeepService();

    Error = QueryCleanupValue(KMDFSTUB_NOUNLOAD_SERVICE_KEY, &Value);
    ok_eq_ulong(Error, (DWORD)ERROR_SUCCESS);
    ok_eq_ulong(Value, 1UL);
}

START_TEST(KmdfStub)
{
    RunKmdfStub();
    RunKmdfStub();
    DeleteTestService(KMDFSTUB_SERVICE_NAME);

    RunKmdfStubNoUnload();
    DeleteTestService(KMDFSTUB_NOUNLOAD_SERVICE_NAME);
}
