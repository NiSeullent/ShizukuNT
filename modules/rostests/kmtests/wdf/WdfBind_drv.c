/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     KMDF bind, version and timer test driver
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <kmt_test.h>
#include <wdf.h>
#include <wdfminiport.h>
#include <wdmsec.h>

#include "WdfBind.h"

typedef struct _WDFBIND_TIMER_CONTEXT
{
    KEVENT Event;
    volatile LONG Count;
} WDFBIND_TIMER_CONTEXT, *PWDFBIND_TIMER_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFBIND_TIMER_CONTEXT, WdfBindGetTimerContext)

static KMT_MESSAGE_HANDLER TestMessageHandler;
static EVT_WDF_TIMER TestEvtTimer;

static WDFDRIVER TestWdfDriver;
static WDFDEVICE TestControlDevice;
static ULONG64 TestFunctionTable;

static
VOID
NTAPI
TestEvtTimer(
    _In_ WDFTIMER Timer)
{
    PWDFBIND_TIMER_CONTEXT Context = WdfBindGetTimerContext(Timer);

    InterlockedIncrement(&Context->Count);
    KeSetEvent(&Context->Event, IO_NO_INCREMENT, FALSE);
}

static
WDFTIMER
CreateTestTimer(
    _In_ ULONG Period,
    _In_ ULONG TolerableDelay,
    _In_ WDF_TRI_STATE UseHighResolutionTimer,
    _In_ NTSTATUS ExpectedStatus)
{
    WDF_TIMER_CONFIG Config;
    WDF_OBJECT_ATTRIBUTES Attributes;
    PWDFBIND_TIMER_CONTEXT Context;
    WDFTIMER Timer = NULL;
    NTSTATUS Status;

    if (Period != 0)
        WDF_TIMER_CONFIG_INIT_PERIODIC(&Config, TestEvtTimer, Period);
    else
        WDF_TIMER_CONFIG_INIT(&Config, TestEvtTimer);

    Config.TolerableDelay = TolerableDelay;
    Config.UseHighResolutionTimer = UseHighResolutionTimer;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&Attributes, WDFBIND_TIMER_CONTEXT);
    Attributes.ParentObject = TestControlDevice;

    Status = WdfTimerCreate(&Config, &Attributes, &Timer);
    ok_eq_hex(Status, ExpectedStatus);
    if (!NT_SUCCESS(Status))
        return NULL;

    Context = WdfBindGetTimerContext(Timer);
    KeInitializeEvent(&Context->Event, SynchronizationEvent, FALSE);
    Context->Count = 0;
    return Timer;
}

static
NTSTATUS
WaitForTestTimer(
    _In_ WDFTIMER Timer)
{
    LARGE_INTEGER Timeout;

    Timeout.QuadPart = -50000000LL;
    return KeWaitForSingleObject(&WdfBindGetTimerContext(Timer)->Event,
                                 Executive,
                                 KernelMode,
                                 FALSE,
                                 &Timeout);
}

static
VOID
TestOneShotTimer(
    _In_ WDF_TRI_STATE UseHighResolutionTimer)
{
    WDFTIMER Timer;
    NTSTATUS Status;

    Timer = CreateTestTimer(0, 0, UseHighResolutionTimer, STATUS_SUCCESS);
    if (skip(Timer != NULL, "No timer\n"))
        return;

    ok_eq_bool(WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(10)), FALSE);
    Status = WaitForTestTimer(Timer);
    ok_eq_hex(Status, STATUS_SUCCESS);
    ok_eq_long(WdfBindGetTimerContext(Timer)->Count, 1L);
    ok_eq_bool(WdfTimerStop(Timer, TRUE), FALSE);
    ok_eq_long(WdfBindGetTimerContext(Timer)->Count, 1L);

    WdfObjectDelete(Timer);
}

static
VOID
TestPeriodicTimer(
    _In_ ULONG Period,
    _In_ ULONG TolerableDelay,
    _In_ WDF_TRI_STATE UseHighResolutionTimer)
{
    WDFTIMER Timer;
    NTSTATUS Status;

    Timer = CreateTestTimer(Period, TolerableDelay, UseHighResolutionTimer, STATUS_SUCCESS);
    if (skip(Timer != NULL, "No timer\n"))
        return;

    ok_eq_bool(WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(Period)), FALSE);
    Status = WaitForTestTimer(Timer);
    ok_eq_hex(Status, STATUS_SUCCESS);
    Status = WaitForTestTimer(Timer);
    ok_eq_hex(Status, STATUS_SUCCESS);
    ok(WdfBindGetTimerContext(Timer)->Count >= 2,
       "Count = %ld, expected at least 2\n", WdfBindGetTimerContext(Timer)->Count);
    ok_eq_bool(WdfTimerStop(Timer, TRUE), TRUE);

    WdfObjectDelete(Timer);
}

static
VOID
TestCancelledTimer(
    _In_ WDF_TRI_STATE UseHighResolutionTimer)
{
    WDFTIMER Timer;

    Timer = CreateTestTimer(0, 0, UseHighResolutionTimer, STATUS_SUCCESS);
    if (skip(Timer != NULL, "No timer\n"))
        return;

    ok_eq_bool(WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(60000)), FALSE);
    ok_eq_bool(WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(60000)), TRUE);
    ok_eq_bool(WdfTimerStop(Timer, TRUE), TRUE);
    ok_eq_bool(WdfTimerStop(Timer, TRUE), FALSE);
    ok_eq_long(WdfBindGetTimerContext(Timer)->Count, 0L);

    WdfObjectDelete(Timer);
}

static
VOID
TestTimers(VOID)
{
    WDFTIMER Timer;

    if (skip(TestControlDevice != NULL, "No control device\n"))
        return;

    TestOneShotTimer(WdfFalse);
    TestOneShotTimer(WdfTrue);
    TestPeriodicTimer(5, 0, WdfFalse);
    TestPeriodicTimer(20, 0, WdfTrue);
    TestPeriodicTimer(5, TolerableDelayUnlimited, WdfFalse);
    TestCancelledTimer(WdfFalse);
    TestCancelledTimer(WdfTrue);

    Timer = CreateTestTimer(0, 10, WdfTrue, STATUS_INVALID_PARAMETER);
    ok_eq_pointer(Timer, NULL);
    if (Timer != NULL)
        WdfObjectDelete(Timer);
}

static
VOID
TestVersion(
    _In_ PWDFBIND_PARAMS Params)
{
    WDF_DRIVER_VERSION_AVAILABLE_PARAMS Version;
    UNICODE_STRING Text;
    WDFSTRING String;
    NTSTATUS Status;

    WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, 1, 0);
    ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), TRUE);

    WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, KMDF_VERSION_MAJOR, KMDF_VERSION_MINOR);
    ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), TRUE);

    WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, 1, 0);
    Version.Size = 0;
    ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), FALSE);

    if (!skip(Params->MajorVersion != 0, "The framework version is not registered\n"))
    {
        WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, Params->MajorVersion, Params->MinorVersion);
        ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), TRUE);

        WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, Params->MajorVersion, Params->MinorVersion + 1);
        ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), FALSE);

        WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&Version, Params->MajorVersion + 1, 0);
        ok_eq_bool(WdfDriverIsVersionAvailable(TestWdfDriver, &Version), FALSE);
    }

    Status = WdfStringCreate(NULL, WDF_NO_OBJECT_ATTRIBUTES, &String);
    ok_eq_hex(Status, STATUS_SUCCESS);
    if (!NT_SUCCESS(Status))
        return;

    Status = WdfDriverRetrieveVersionString(TestWdfDriver, String);
    ok_eq_hex(Status, STATUS_SUCCESS);
    WdfStringGetUnicodeString(String, &Text);
    ok(Text.Length != 0, "Empty framework version string\n");

    WdfObjectDelete(String);
}

NTSTATUS
TestEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PCUNICODE_STRING RegistryPath,
    OUT PCWSTR *DeviceName,
    IN OUT INT *Flags)
{
    WDF_DRIVER_CONFIG Config;
    PWDFDEVICE_INIT DeviceInit;
    NTSTATUS Status;
    ULONG Missing;
    ULONG Index;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Flags);

    *DeviceName = L"WdfBind";

    ok(WdfDriverGlobals != NULL, "WdfDriverGlobals is NULL\n");
    ok(WdfFunctions != NULL, "WdfFunctions is NULL\n");
    if (WdfDriverGlobals == NULL || WdfFunctions == NULL)
        return STATUS_UNSUCCESSFUL;

    TestFunctionTable = (ULONG_PTR)WdfFunctions;
    Missing = 0;
    for (Index = 0; Index < WdfFunctionTableNumEntries; Index++)
    {
        if (WdfFunctions[Index] == NULL)
            Missing++;
    }
    ok_eq_ulong(Missing, 0UL);

    WDF_DRIVER_CONFIG_INIT(&Config, WDF_NO_EVENT_CALLBACK);
    Config.DriverInitFlags = WdfDriverInitNonPnpDriver | WdfDriverInitNoDispatchOverride;
    Status = WdfDriverCreate(DriverObject,
                             (PUNICODE_STRING)RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &Config,
                             &TestWdfDriver);
    ok_eq_hex(Status, STATUS_SUCCESS);
    if (!NT_SUCCESS(Status))
    {
        TestWdfDriver = NULL;
        return Status;
    }

    DeviceInit = WdfControlDeviceInitAllocate(TestWdfDriver, &SDDL_DEVOBJ_SYS_ALL_ADM_ALL);
    ok(DeviceInit != NULL, "WdfControlDeviceInitAllocate failed\n");
    if (DeviceInit != NULL)
    {
        Status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &TestControlDevice);
        ok_eq_hex(Status, STATUS_SUCCESS);
        if (NT_SUCCESS(Status))
        {
            WdfControlFinishInitializing(TestControlDevice);
        }
        else
        {
            WdfDeviceInitFree(DeviceInit);
            TestControlDevice = NULL;
        }
    }

    KmtRegisterMessageHandler(IOCTL_WDFBIND_RUN, NULL, TestMessageHandler);
    return STATUS_SUCCESS;
}

VOID
TestUnload(
    IN PDRIVER_OBJECT DriverObject)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(DriverObject);

    if (TestControlDevice != NULL)
    {
        WdfObjectDelete(TestControlDevice);
        TestControlDevice = NULL;
    }

    if (TestWdfDriver != NULL)
    {
        WdfDriverMiniportUnload(TestWdfDriver);
        TestWdfDriver = NULL;
    }
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
    WDFBIND_PARAMS Params;
    SIZE_T OutputLength = *OutLength;

    UNREFERENCED_PARAMETER(DeviceObject);

    *OutLength = 0;

    if (ControlCode != IOCTL_WDFBIND_RUN || Buffer == NULL ||
        InLength < sizeof(Params) || OutputLength < sizeof(Params))
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlCopyMemory(&Params, Buffer, sizeof(Params));

    ok_irql(PASSIVE_LEVEL);
    TestVersion(&Params);
    TestTimers();

    Params.FunctionTable = TestFunctionTable;
    RtlCopyMemory(Buffer, &Params, sizeof(Params));
    *OutLength = sizeof(Params);
    return STATUS_SUCCESS;
}
