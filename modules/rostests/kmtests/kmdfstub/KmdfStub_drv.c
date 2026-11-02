/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Permissive KMDF client stub test driver
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <kmt_test.h>
#include <wdf.h>

#include "KmdfStub.h"

static KMT_MESSAGE_HANDLER TestMessageHandler;
static EVT_WDF_DRIVER_UNLOAD TestEvtDriverUnload;
static EVT_WDF_OBJECT_CONTEXT_CLEANUP TestEvtDriverCleanup;

static PDRIVER_OBJECT TestDriverObject;
static WDFDRIVER TestWdfDriver;
static WCHAR TestServiceKey[256];

static
VOID
NTAPI
TestEvtDriverUnload(
    _In_ WDFDRIVER Driver)
{
    UNREFERENCED_PARAMETER(Driver);
}

static
VOID
NTAPI
TestEvtDriverCleanup(
    _In_ WDFOBJECT Object)
{
    ULONG Value = 1;

    UNREFERENCED_PARAMETER(Object);

    if (TestServiceKey[0] != UNICODE_NULL)
    {
        RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE,
                              TestServiceKey,
                              KMDFSTUB_CLEANUP_VALUE,
                              REG_DWORD,
                              &Value,
                              sizeof(Value));
    }
}

NTSTATUS
TestEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PCUNICODE_STRING RegistryPath,
    OUT PCWSTR *DeviceName,
    IN OUT INT *Flags)
{
    WDF_DRIVER_CONFIG Config;
    WDF_OBJECT_ATTRIBUTES Attributes;
    NTSTATUS Status;
    ULONG Missing;
    ULONG Index;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Flags);

    *DeviceName = L"KmdfStub";
    TestDriverObject = DriverObject;

    RtlZeroMemory(TestServiceKey, sizeof(TestServiceKey));
    ok(RegistryPath->Length < sizeof(TestServiceKey), "Registry path too long: %u\n", RegistryPath->Length);
    if (RegistryPath->Length < sizeof(TestServiceKey))
        RtlCopyMemory(TestServiceKey, RegistryPath->Buffer, RegistryPath->Length);

    ok(WdfDriverGlobals != NULL, "WdfDriverGlobals is NULL\n");
    ok(WdfFunctions != NULL, "WdfFunctions is NULL\n");
    if (WdfDriverGlobals == NULL || WdfFunctions == NULL)
        return STATUS_UNSUCCESSFUL;

    Missing = 0;
    for (Index = 0; Index < WdfFunctionTableNumEntries; Index++)
    {
        if (WdfFunctions[Index] == NULL)
            Missing++;
    }
    ok_eq_ulong(Missing, 0UL);

    WDF_DRIVER_CONFIG_INIT(&Config, WDF_NO_EVENT_CALLBACK);
    Config.DriverInitFlags = WdfDriverInitNonPnpDriver | WdfDriverInitNoDispatchOverride;
    Config.EvtDriverUnload = TestEvtDriverUnload;

    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
    Attributes.EvtCleanupCallback = TestEvtDriverCleanup;

    Status = WdfDriverCreate(DriverObject,
                             (PUNICODE_STRING)RegistryPath,
                             &Attributes,
                             &Config,
                             &TestWdfDriver);
    ok_eq_hex(Status, STATUS_SUCCESS);
    if (!NT_SUCCESS(Status))
    {
        TestWdfDriver = NULL;
        return Status;
    }

    ok_eq_pointer(WdfDriverGlobals->Driver, TestWdfDriver);
    ok_eq_bool(WdfDriverGlobals->DisplaceDriverUnload, TRUE);

    KmtRegisterMessageHandler(IOCTL_KMDFSTUB_CHECK, NULL, TestMessageHandler);
    return STATUS_SUCCESS;
}

VOID
TestUnload(
    IN PDRIVER_OBJECT DriverObject)
{
    PAGED_CODE();

    ok_eq_pointer(DriverObject, TestDriverObject);
    ok(WdfDriverGlobals != NULL, "Framework unbound before the captured unload routine ran\n");
    if (WdfDriverGlobals != NULL && TestWdfDriver != NULL)
        ok_eq_pointer(WdfGetDriver(), TestWdfDriver);
}

static
NTSTATUS
TestMessageHandler(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG ControlCode,
    IN PVOID Buffer OPTIONAL,
    IN SIZE_T InLength,
    IN OUT PSIZE_T OutLength)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(InLength);

    *OutLength = 0;

    if (ControlCode != IOCTL_KMDFSTUB_CHECK)
        return STATUS_INVALID_PARAMETER;

    ok_irql(PASSIVE_LEVEL);

    if (skip(TestWdfDriver != NULL && WdfDriverGlobals != NULL, "No framework driver object\n"))
        return STATUS_SUCCESS;

    ok_eq_pointer(WdfGetDriver(), TestWdfDriver);
    ok_eq_pointer(WdfDriverWdmGetDriverObject(TestWdfDriver), TestDriverObject);
    ok(TestDriverObject->DriverUnload != NULL, "DriverUnload is NULL\n");
    ok_eq_bool(WdfDriverGlobals->DisplaceDriverUnload, TRUE);
    return STATUS_SUCCESS;
}
