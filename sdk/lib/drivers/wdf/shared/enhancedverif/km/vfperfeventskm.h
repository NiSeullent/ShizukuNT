/*
 * PROJECT:     LiberNT Kernel-Mode Driver Framework
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Performance analysis event writers used by the enhanced verifier
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#pragma once

/* TODO: emit the DriverFrameworks-KernelMode-Performance ETW events; the provider manifest is not public */

FORCEINLINE
NTSTATUS
VfPerfEventWriteRequest(
    _In_ LPCGUID Activity,
    _In_ UCHAR MajorFunction,
    _In_ PFN_WDF_DRIVER_DEVICE_ADD DriverDeviceAdd,
    _In_ WDFDEVICE Device
    )
{
    UNREFERENCED_PARAMETER(Activity);
    UNREFERENCED_PARAMETER(MajorFunction);
    UNREFERENCED_PARAMETER(DriverDeviceAdd);
    UNREFERENCED_PARAMETER(Device);
    return STATUS_SUCCESS;
}

FORCEINLINE
NTSTATUS
VfPerfEventWriteCallback(
    _In_ LPCGUID Activity,
    _In_ PFN_WDF_DRIVER_DEVICE_ADD DriverDeviceAdd,
    _In_ WDFDEVICE Device
    )
{
    UNREFERENCED_PARAMETER(Activity);
    UNREFERENCED_PARAMETER(DriverDeviceAdd);
    UNREFERENCED_PARAMETER(Device);
    return STATUS_SUCCESS;
}

#define EventWriteFX_REQUEST_START VfPerfEventWriteRequest
#define EventWriteFX_REQUEST_COMPLETE VfPerfEventWriteRequest
#define EventWriteFX_POWER_D0_ENTRY_START VfPerfEventWriteCallback
#define EventWriteFX_POWER_D0_ENTRY_STOP VfPerfEventWriteCallback
#define EventWriteFX_POWER_D0_EXIT_START VfPerfEventWriteCallback
#define EventWriteFX_POWER_D0_EXIT_STOP VfPerfEventWriteCallback
#define EventWriteFX_POWER_HW_PREPARE_START VfPerfEventWriteCallback
#define EventWriteFX_POWER_HW_PREPARE_STOP VfPerfEventWriteCallback
#define EventWriteFX_POWER_HW_RELEASE_START VfPerfEventWriteCallback
#define EventWriteFX_POWER_HW_RELEASE_STOP VfPerfEventWriteCallback
#define EventWriteFX_EVTIOSTOP_START VfPerfEventWriteCallback
#define EventWriteFX_EVTIOSTOP_STOP VfPerfEventWriteCallback
