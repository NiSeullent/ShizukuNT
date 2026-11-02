/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Permissive KMDF client stub test driver without its own unload routine
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <ntddk.h>
#include <wdf.h>

#include "KmdfStub.h"

DRIVER_INITIALIZE DriverEntry;
static EVT_WDF_DRIVER_UNLOAD NoUnloadEvtDriverUnload;
static EVT_WDF_OBJECT_CONTEXT_CLEANUP NoUnloadEvtDriverCleanup;

static WCHAR NoUnloadServiceKey[256];

static
VOID
NTAPI
NoUnloadEvtDriverUnload(
    _In_ WDFDRIVER Driver)
{
    UNREFERENCED_PARAMETER(Driver);
}

static
VOID
NTAPI
NoUnloadEvtDriverCleanup(
    _In_ WDFOBJECT Object)
{
    ULONG Value = 1;

    UNREFERENCED_PARAMETER(Object);

    RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE,
                          NoUnloadServiceKey,
                          KMDFSTUB_CLEANUP_VALUE,
                          REG_DWORD,
                          &Value,
                          sizeof(Value));
}

NTSTATUS
NTAPI
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG Config;
    WDF_OBJECT_ATTRIBUTES Attributes;

    PAGED_CODE();

    if (RegistryPath->Length >= sizeof(NoUnloadServiceKey))
        return STATUS_NAME_TOO_LONG;

    RtlZeroMemory(NoUnloadServiceKey, sizeof(NoUnloadServiceKey));
    RtlCopyMemory(NoUnloadServiceKey, RegistryPath->Buffer, RegistryPath->Length);

    WDF_DRIVER_CONFIG_INIT(&Config, WDF_NO_EVENT_CALLBACK);
    Config.DriverInitFlags = WdfDriverInitNonPnpDriver | WdfDriverInitNoDispatchOverride;
    Config.EvtDriverUnload = NoUnloadEvtDriverUnload;

    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
    Attributes.EvtCleanupCallback = NoUnloadEvtDriverCleanup;

    return WdfDriverCreate(DriverObject, RegistryPath, &Attributes, &Config, WDF_NO_HANDLE);
}
