/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     NdisMRegisterWdiMiniportDriver WDI version acceptance tests
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#define NDIS_MINIPORT_DRIVER 1
#define NDIS650_MINIPORT 1
#include <kmt_test.h>
#include <ndis.h>
#include <dot11wdi.h>

NTKERNELAPI
NTSTATUS
NTAPI
IoCreateDriver(
    _In_opt_ PUNICODE_STRING DriverName,
    _In_ PDRIVER_INITIALIZE InitializationFunction);

NTKERNELAPI
VOID
NTAPI
IoDeleteDriver(
    _In_ PDRIVER_OBJECT DriverObject);

static ULONG TestWdiVersion;
static NDIS_STATUS TestRegisterStatus;
static BOOLEAN TestHandleReturned;
static PDRIVER_OBJECT TestDriverObject;

static
NDIS_STATUS
NTAPI
TestAllocateAdapter(
    _In_ NDIS_HANDLE MiniportAdapterHandle,
    _In_ NDIS_HANDLE MiniportDriverContext,
    _In_ PNDIS_MINIPORT_INIT_PARAMETERS MiniportInitParameters,
    _In_ PNDIS_WDI_INIT_PARAMETERS NdisWdiInitParameters,
    _In_ PNDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES RegistrationAttributes)
{
    UNREFERENCED_PARAMETER(MiniportAdapterHandle);
    UNREFERENCED_PARAMETER(MiniportDriverContext);
    UNREFERENCED_PARAMETER(MiniportInitParameters);
    UNREFERENCED_PARAMETER(NdisWdiInitParameters);
    UNREFERENCED_PARAMETER(RegistrationAttributes);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
VOID
NTAPI
TestAdapterVoid(
    _In_ NDIS_HANDLE MiniportAdapterContext)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
}

static
NDIS_STATUS
NTAPI
TestAdapterStatus(
    _In_ NDIS_HANDLE MiniportAdapterContext)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestOpenAdapter(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ PNDIS_MINIPORT_INIT_PARAMETERS MiniportInitParameters)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(MiniportInitParameters);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestPostPause(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ PNDIS_MINIPORT_PAUSE_PARAMETERS PauseParameters)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(PauseParameters);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestPostRestart(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ PNDIS_MINIPORT_RESTART_PARAMETERS RestartParameters)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(RestartParameters);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestHangDiagnose(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ eDiagnoseLevel DiagnoseLevel,
    _In_ UINT32 BufferSize,
    _Out_ UINT8 *FirmwareBlock,
    _Out_ UINT32 *BytesWritten)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(DiagnoseLevel);
    UNREFERENCED_PARAMETER(BufferSize);
    UNREFERENCED_PARAMETER(FirmwareBlock);
    *BytesWritten = 0;
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestTalTxRxInitialize(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ PNDIS_WDI_DATA_API NdisWdiDataPathApi,
    _Out_ PTAL_TXRX_HANDLE MiniportTalTxRxContext,
    _Out_ PNDIS_MINIPORT_WDI_DATA_HANDLERS MiniportWdiDataHandlers,
    _Out_ UINT32 *MiniportWdiFrameMetadataExtraSpace)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(NdisMiniportDataPathHandle);
    UNREFERENCED_PARAMETER(NdisWdiDataPathApi);
    UNREFERENCED_PARAMETER(MiniportTalTxRxContext);
    UNREFERENCED_PARAMETER(MiniportWdiDataHandlers);
    *MiniportWdiFrameMetadataExtraSpace = 0;
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
VOID
NTAPI
TestTalTxRxDeinitialize(
    _In_ TAL_TXRX_HANDLE MiniportTalTxRxContext)
{
    UNREFERENCED_PARAMETER(MiniportTalTxRxContext);
}

static
NDIS_STATUS
NTAPI
TestIdleNotification(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ BOOLEAN ForceLowPower)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(ForceLowPower);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
NDIS_STATUS
NTAPI
TestOidRequest(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ PNDIS_OID_REQUEST OidRequest)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(OidRequest);
    return NDIS_STATUS_NOT_SUPPORTED;
}

static
VOID
NTAPI
TestCancelOidRequest(
    _In_ NDIS_HANDLE MiniportAdapterContext,
    _In_ PVOID RequestId)
{
    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(RequestId);
}

static
NTSTATUS
NTAPI
TestWdiDriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath)
{
    UNICODE_STRING ServicePath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\Kmtest");
    NDIS_MINIPORT_DRIVER_CHARACTERISTICS Characteristics;
    NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS Wdi;
    NDIS_HANDLE Handle = NULL;

    UNREFERENCED_PARAMETER(RegistryPath);

    ObReferenceObject(DriverObject);
    TestDriverObject = DriverObject;

    RtlZeroMemory(&Characteristics, sizeof(Characteristics));
    Characteristics.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_DRIVER_CHARACTERISTICS;
    Characteristics.Header.Revision = NDIS_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_2;
    Characteristics.Header.Size = NDIS_SIZEOF_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_2;
    Characteristics.MajorNdisVersion = 6;
    Characteristics.MinorNdisVersion = 50;
    Characteristics.OidRequestHandler = TestOidRequest;
    Characteristics.CancelOidRequestHandler = TestCancelOidRequest;

    RtlZeroMemory(&Wdi, sizeof(Wdi));
    Wdi.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_WDI_CHARACTERISTICS;
    Wdi.Header.Revision = NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS_REVISION_1;
    Wdi.Header.Size = NDIS_SIZEOF_MINIPORT_WDI_CHARACTERISTICS_REVISION_1;
    Wdi.WdiVersion = TestWdiVersion;
    Wdi.AllocateAdapterHandler = TestAllocateAdapter;
    Wdi.FreeAdapterHandler = TestAdapterVoid;
    Wdi.OpenAdapterHandler = TestOpenAdapter;
    Wdi.CloseAdapterHandler = TestAdapterStatus;
    Wdi.StartOperationHandler = TestAdapterStatus;
    Wdi.StopOperationHandler = TestAdapterVoid;
    Wdi.PostPauseHandler = TestPostPause;
    Wdi.PostRestartHandler = TestPostRestart;
    Wdi.HangDiagnoseHandler = TestHangDiagnose;
    Wdi.TalTxRxInitializeHandler = TestTalTxRxInitialize;
    Wdi.TalTxRxDeinitializeHandler = TestTalTxRxDeinitialize;
    Wdi.LeIdleNotificationHandler = TestIdleNotification;
    Wdi.LeCancelIdleNotificationHandler = TestAdapterVoid;

    TestRegisterStatus = NdisMRegisterWdiMiniportDriver(DriverObject, &ServicePath, NULL, &Characteristics, &Wdi, &Handle);
    TestHandleReturned = (Handle != NULL);
    if (TestRegisterStatus == NDIS_STATUS_SUCCESS && Handle != NULL)
        NdisMDeregisterWdiMiniportDriver(Handle);

    return STATUS_SUCCESS;
}

static
NDIS_STATUS
TestRegister(
    _In_ ULONG WdiVersion,
    _Out_ PBOOLEAN HandleReturned)
{
    NTSTATUS Status;

    TestWdiVersion = WdiVersion;
    TestRegisterStatus = NDIS_STATUS_FAILURE;
    TestHandleReturned = FALSE;
    TestDriverObject = NULL;
    *HandleReturned = FALSE;

    Status = IoCreateDriver(NULL, TestWdiDriverEntry);
    ok_eq_hex(Status, STATUS_SUCCESS);
    if (TestDriverObject != NULL)
    {
        if (NT_SUCCESS(Status))
        {
            ObMakeTemporaryObject(TestDriverObject);
            IoDeleteDriver(TestDriverObject);
        }
        ObDereferenceObject(TestDriverObject);
        TestDriverObject = NULL;
    }

    *HandleReturned = TestHandleReturned;
    return TestRegisterStatus;
}

START_TEST(NdisWdiVersion)
{
    static const ULONG Accepted[] =
    {
        WDI_VERSION_1_0,
        WDI_VERSION_1_0_1,
        WDI_VERSION_1_0_21,
        WDI_VERSION_1_1_12,
        WDI_VERSION_1_1_13,
        (1 << 16) | (1 << 8) | 0xe,
        (1 << 16) | (2 << 8) | 0x0,
        (1 << 16) | 0xffff,
    };
    static const ULONG Rejected[] =
    {
        0,
        WDI_VERSION_1_0 - 1,
        (2 << 16) | (0 << 8) | 0x0,
        (2 << 16) | (0 << 8) | 0xe,
        MAXULONG,
    };
    BOOLEAN HandleReturned;
    NDIS_STATUS Status;
    ULONG i;

    ok_eq_hex(WDI_VERSION_1_1_13, 0x1010dUL);
    ok_eq_hex(WDI_VERSION_LATEST, WDI_VERSION_1_1_13);

    for (i = 0; i < RTL_NUMBER_OF(Accepted); i++)
    {
        Status = TestRegister(Accepted[i], &HandleReturned);
        ok(Status != NDIS_STATUS_BAD_VERSION, "WdiVersion 0x%lx was rejected as a bad version\n", Accepted[i]);
        ok(Status == NDIS_STATUS_SUCCESS, "WdiVersion 0x%lx: status 0x%lx\n", Accepted[i], Status);
        ok(HandleReturned == (Status == NDIS_STATUS_SUCCESS), "WdiVersion 0x%lx: handle %u\n", Accepted[i], HandleReturned);
    }

    for (i = 0; i < RTL_NUMBER_OF(Rejected); i++)
    {
        Status = TestRegister(Rejected[i], &HandleReturned);
        ok(Status == NDIS_STATUS_BAD_VERSION, "WdiVersion 0x%lx: status 0x%lx\n", Rejected[i], Status);
        ok(!HandleReturned, "WdiVersion 0x%lx returned a driver handle\n", Rejected[i]);
    }
}
