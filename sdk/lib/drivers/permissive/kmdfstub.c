/*
 * PROJECT:     LiberNT permissive driver runtime
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     KMDF client entry stub for drivers that must not link GPL code
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <ntddk.h>
#include <wdf.h>

#define KMDFSTUB_TAG 'bSdK'

typedef struct _WDF_VERSION
{
    ULONG Major;
    ULONG Minor;
    ULONG Build;
} WDF_VERSION;

typedef struct _WDF_BIND_INFO
{
    ULONG Size;
    PWCHAR Component;
    WDF_VERSION Version;
    ULONG FuncCount;
    WDFFUNC *FuncTable;
    PVOID Module;
} WDF_BIND_INFO, *PWDF_BIND_INFO;

NTSTATUS
NTAPI
WdfVersionBind(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath,
    _Inout_ PWDF_BIND_INFO BindInfo,
    _Out_ PWDF_DRIVER_GLOBALS *ComponentGlobals);

NTSTATUS
NTAPI
WdfVersionUnbind(
    _In_ PUNICODE_STRING RegistryPath,
    _In_ PWDF_BIND_INFO BindInfo,
    _In_ PWDF_DRIVER_GLOBALS ComponentGlobals);

DRIVER_INITIALIZE DriverEntry;
DRIVER_INITIALIZE FxDriverEntry;
static DRIVER_UNLOAD KmdfStubDriverUnload;

const WDFFUNC *WdfFunctions = NULL;
PWDF_DRIVER_GLOBALS WdfDriverGlobals = NULL;

static WCHAR KmdfStubComponent[] = L"KmdfLibrary";

/* From KMDF 1.15 on, the framework stores the address of its function table through FuncTable. */
static WDF_BIND_INFO KmdfStubBindInfo =
{
    sizeof(WDF_BIND_INFO),
    KmdfStubComponent,
    { KMDF_VERSION_MAJOR, KMDF_VERSION_MINOR, 0 },
    WdfFunctionTableNumEntries,
    (WDFFUNC *)&WdfFunctions,
    NULL
};

C_ASSERT(KMDF_VERSION_MAJOR == 1 && KMDF_VERSION_MINOR >= 15 && KMDF_VERSION_MINOR < 25);

static UNICODE_STRING KmdfStubRegistryPath;
static PDRIVER_UNLOAD KmdfStubFrameworkUnload;

static
NTSTATUS
KmdfStubSaveRegistryPath(
    _In_ PCUNICODE_STRING RegistryPath)
{
    KmdfStubRegistryPath.Buffer = ExAllocatePool2(POOL_FLAG_PAGED,
                                                  (SIZE_T)RegistryPath->Length + sizeof(UNICODE_NULL),
                                                  KMDFSTUB_TAG);
    if (KmdfStubRegistryPath.Buffer == NULL)
        return STATUS_INSUFFICIENT_RESOURCES;

    KmdfStubRegistryPath.Length = 0;
    KmdfStubRegistryPath.MaximumLength = RegistryPath->Length;
    RtlCopyUnicodeString(&KmdfStubRegistryPath, RegistryPath);
    return STATUS_SUCCESS;
}

static
VOID
KmdfStubFreeRegistryPath(VOID)
{
    if (KmdfStubRegistryPath.Buffer != NULL)
        ExFreePoolWithTag(KmdfStubRegistryPath.Buffer, KMDFSTUB_TAG);

    RtlInitEmptyUnicodeString(&KmdfStubRegistryPath, NULL, 0);
}

static
VOID
KmdfStubUnbind(VOID)
{
    WdfVersionUnbind(&KmdfStubRegistryPath, &KmdfStubBindInfo, WdfDriverGlobals);
    WdfDriverGlobals = NULL;
    KmdfStubFreeRegistryPath();
}

static
VOID
NTAPI
KmdfStubDriverUnload(
    _In_ PDRIVER_OBJECT DriverObject)
{
    PDRIVER_UNLOAD FrameworkUnload = KmdfStubFrameworkUnload;

    PAGED_CODE();

    KmdfStubFrameworkUnload = NULL;
    if (FrameworkUnload != NULL)
        FrameworkUnload(DriverObject);
    KmdfStubUnbind();
}

NTSTATUS
NTAPI
FxDriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS Status;

    PAGED_CODE();

    Status = KmdfStubSaveRegistryPath(RegistryPath);
    if (!NT_SUCCESS(Status))
        return Status;

    Status = WdfVersionBind(DriverObject, &KmdfStubRegistryPath, &KmdfStubBindInfo, &WdfDriverGlobals);
    if (!NT_SUCCESS(Status))
    {
        WdfDriverGlobals = NULL;
        KmdfStubFreeRegistryPath();
        return Status;
    }

    Status = DriverEntry(DriverObject, RegistryPath);
    if (!NT_SUCCESS(Status))
    {
        KmdfStubUnbind();
        return Status;
    }

    if (WdfDriverGlobals->DisplaceDriverUnload)
    {
        KmdfStubFrameworkUnload = DriverObject->DriverUnload;
        DriverObject->DriverUnload = KmdfStubDriverUnload;
    }

    return Status;
}
