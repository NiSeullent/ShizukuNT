/*
 * PROJECT:     LiberNT NT Layer DLL
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     PE Control Flow Guard loader support
 */

#include <ntdll.h>

#define NDEBUG
#include <debug.h>

extern PLDR_DATA_TABLE_ENTRY LdrpImageEntry;

static BOOLEAN LdrpGuardEnabled;
static BOOLEAN LdrpGuardBypass;

static BOOLEAN
LdrpGuardAddressInImage(
    _In_ ULONG_PTR Address,
    _In_ PLDR_DATA_TABLE_ENTRY LdrEntry,
    _In_ SIZE_T Size)
{
    ULONG_PTR Base = (ULONG_PTR)LdrEntry->DllBase;

    return Address >= Base &&
           Address - Base <= LdrEntry->SizeOfImage &&
           Size <= LdrEntry->SizeOfImage - (Address - Base);
}

static PIMAGE_LOAD_CONFIG_DIRECTORY
LdrpGuardGetLoadConfig(
    _In_ PLDR_DATA_TABLE_ENTRY LdrEntry,
    _In_ PIMAGE_NT_HEADERS NtHeaders,
    _Out_ PULONG ConfigSize)
{
    PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig;

    *ConfigSize = 0;
    if (NtHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
        return NULL;

    LoadConfig = RtlImageDirectoryEntryToData(LdrEntry->DllBase, TRUE, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, ConfigSize);
    if (LoadConfig == NULL)
        return NULL;

    /* PE32 images may keep the 64-byte directory size Windows XP requires and give the real size in the structure. */
    if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
        *ConfigSize == 64 &&
        LoadConfig->Size > *ConfigSize &&
        LdrpGuardAddressInImage((ULONG_PTR)LoadConfig, LdrEntry, LoadConfig->Size))
    {
        *ConfigSize = LoadConfig->Size;
    }

    return LoadConfig;
}

#define LDRP_GUARD_CACHE_SLOTS 16
#define LDRP_GUARD_CACHE_RANGES 4

typedef struct _LDRP_GUARD_MODULE_INFO
{
    volatile LONG Sequence;
    LONG Generation;
    ULONG_PTR ImageBase;
    ULONG_PTR ImageEnd;
    ULONG RangeCount;
    ULONG_PTR RangeStart[LDRP_GUARD_CACHE_RANGES];
    ULONG_PTR RangeEnd[LDRP_GUARD_CACHE_RANGES];
    PIMAGE_SECTION_HEADER SectionTable;
    ULONG SectionCount;
    BOOLEAN Enforced;
    BOOLEAN Usable;
    ULONG_PTR GuardTable;
    ULONGLONG GuardCount;
    SIZE_T GuardStride;
} LDRP_GUARD_MODULE_INFO, *PLDRP_GUARD_MODULE_INFO;

static LDRP_GUARD_MODULE_INFO LdrpGuardModuleCache[LDRP_GUARD_CACHE_SLOTS];
static volatile LONG LdrpGuardCacheGeneration = 1;
static volatile LONG LdrpGuardCacheVictim;
static volatile LONG LdrpGuardCacheHint;

VOID
NTAPI
LdrpGuardInvalidateModuleCache(VOID)
{
    InterlockedIncrement(&LdrpGuardCacheGeneration);
}

static BOOLEAN
LdrpGuardReadCachedModule(
    _In_ ULONG_PTR Target,
    _Out_ PLDRP_GUARD_MODULE_INFO Result)
{
    LONG Generation = LdrpGuardCacheGeneration;
    ULONG Start = (ULONG)LdrpGuardCacheHint;
    ULONG Probe;

    for (Probe = 0; Probe < LDRP_GUARD_CACHE_SLOTS; ++Probe)
    {
        ULONG Index = (Start + Probe) & (LDRP_GUARD_CACHE_SLOTS - 1);
        PLDRP_GUARD_MODULE_INFO Slot = &LdrpGuardModuleCache[Index];
        LONG First;

        First = Slot->Sequence;
        if ((First & 1) != 0)
            continue;

        if (Slot->Generation != Generation || !Slot->Usable)
            continue;

        if (Target < Slot->ImageBase || Target >= Slot->ImageEnd)
            continue;

        MemoryBarrier();
        *Result = *Slot;
        MemoryBarrier();

        if (Slot->Sequence != First)
            continue;

        LdrpGuardCacheHint = (LONG)Index;
        return TRUE;
    }

    return FALSE;
}

static VOID
LdrpGuardPublishCachedModule(
    _In_ PLDRP_GUARD_MODULE_INFO Info)
{
    PLDRP_GUARD_MODULE_INFO Slot;
    ULONG Index;

    Index = ((ULONG)InterlockedIncrement(&LdrpGuardCacheVictim)) & (LDRP_GUARD_CACHE_SLOTS - 1);
    Slot = &LdrpGuardModuleCache[Index];

    InterlockedIncrement(&Slot->Sequence);
    MemoryBarrier();

    Slot->Generation = Info->Generation;
    Slot->ImageBase = Info->ImageBase;
    Slot->ImageEnd = Info->ImageEnd;
    Slot->RangeCount = Info->RangeCount;
    RtlCopyMemory(Slot->RangeStart, Info->RangeStart, sizeof(Slot->RangeStart));
    RtlCopyMemory(Slot->RangeEnd, Info->RangeEnd, sizeof(Slot->RangeEnd));
    Slot->SectionTable = Info->SectionTable;
    Slot->SectionCount = Info->SectionCount;
    Slot->Enforced = Info->Enforced;
    Slot->Usable = Info->Usable;
    Slot->GuardTable = Info->GuardTable;
    Slot->GuardCount = Info->GuardCount;
    Slot->GuardStride = Info->GuardStride;

    MemoryBarrier();
    InterlockedIncrement(&Slot->Sequence);
}

static BOOLEAN
LdrpGuardDescribeModule(
    _In_ PLDR_DATA_TABLE_ENTRY LdrEntry,
    _Out_ PLDRP_GUARD_MODULE_INFO Info)
{
    PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig;
    PIMAGE_SECTION_HEADER Section;
    PIMAGE_NT_HEADERS NtHeaders;
    ULONG ConfigSize;
    ULONG Index;

    RtlZeroMemory(Info, sizeof(*Info));
    Info->Generation = LdrpGuardCacheGeneration;
    Info->ImageBase = (ULONG_PTR)LdrEntry->DllBase;
    Info->ImageEnd = Info->ImageBase + LdrEntry->SizeOfImage;

    NtHeaders = RtlImageNtHeader(LdrEntry->DllBase);
    if (NtHeaders == NULL)
        return FALSE;

    Section = IMAGE_FIRST_SECTION(NtHeaders);
    for (Index = 0; Index < NtHeaders->FileHeader.NumberOfSections; ++Index, ++Section)
    {
        ULONG SectionSize;

        if (!(Section->Characteristics & IMAGE_SCN_MEM_EXECUTE))
            continue;

        if (Info->RangeCount == LDRP_GUARD_CACHE_RANGES)
        {
            Info->RangeCount = 0;
            Info->SectionTable = IMAGE_FIRST_SECTION(NtHeaders);
            Info->SectionCount = NtHeaders->FileHeader.NumberOfSections;
            break;
        }

        SectionSize = max(Section->Misc.VirtualSize, Section->SizeOfRawData);
        Info->RangeStart[Info->RangeCount] = Info->ImageBase + Section->VirtualAddress;
        Info->RangeEnd[Info->RangeCount] = Info->RangeStart[Info->RangeCount] + SectionSize;
        Info->RangeCount++;
    }

    LoadConfig = LdrpGuardGetLoadConfig(LdrEntry, NtHeaders, &ConfigSize);
    if (!(NtHeaders->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF) ||
        LoadConfig == NULL ||
        ConfigSize < RTL_SIZEOF_THROUGH_FIELD(IMAGE_LOAD_CONFIG_DIRECTORY, GuardFlags) ||
        !(LoadConfig->GuardFlags & IMAGE_GUARD_CF_INSTRUMENTED))
    {
        Info->Enforced = FALSE;
        Info->Usable = TRUE;
        return TRUE;
    }

    Info->Enforced = (LoadConfig->GuardFlags & IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT) != 0;

    if (Info->Enforced)
    {
        SIZE_T Stride = sizeof(ULONG) +
                        ((LoadConfig->GuardFlags & IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_MASK) >>
                         IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_SHIFT);
        ULONGLONG Count = LoadConfig->GuardCFFunctionCount;

        if (Stride >= sizeof(ULONG) &&
            Count != 0 &&
            Count <= MAXULONG_PTR / Stride &&
            LdrpGuardAddressInImage((ULONG_PTR)LoadConfig->GuardCFFunctionTable,
                                    LdrEntry, (SIZE_T)Count * Stride))
        {
            Info->GuardTable = (ULONG_PTR)LoadConfig->GuardCFFunctionTable;
            Info->GuardCount = Count;
            Info->GuardStride = Stride;
        }
    }

    Info->Usable = TRUE;
    return TRUE;
}

static BOOLEAN
LdrpGuardTargetIsExecutable(
    _In_ ULONG_PTR Target,
    _In_ PLDRP_GUARD_MODULE_INFO Info)
{
    PIMAGE_SECTION_HEADER Section = Info->SectionTable;
    ULONG Index;

    for (Index = 0; Index < Info->RangeCount; ++Index)
    {
        if (Target >= Info->RangeStart[Index] && Target < Info->RangeEnd[Index])
            return TRUE;
    }

    for (Index = 0; Index < Info->SectionCount; ++Index, ++Section)
    {
        ULONG_PTR SectionStart = Info->ImageBase + Section->VirtualAddress;

        if ((Section->Characteristics & IMAGE_SCN_MEM_EXECUTE) &&
            Target >= SectionStart &&
            Target - SectionStart < max(Section->Misc.VirtualSize, Section->SizeOfRawData))
        {
            return TRUE;
        }
    }

    return FALSE;
}

static BOOLEAN
LdrpGuardEvaluateCached(
    _In_ ULONG_PTR Target,
    _In_ PLDRP_GUARD_MODULE_INFO Info)
{
    const BYTE *Table;
    ULONGLONG Low, High;
    ULONG TargetRva;

    if (!LdrpGuardTargetIsExecutable(Target, Info))
        return FALSE;

    if (!Info->Enforced)
        return TRUE;

    if (Info->GuardTable == 0)
        return FALSE;

    TargetRva = (ULONG)(Target - Info->ImageBase);
    Table = (const BYTE *)Info->GuardTable;
    Low = 0;
    High = Info->GuardCount;
    while (Low < High)
    {
        ULONGLONG Middle = Low + (High - Low) / 2;
        const BYTE *Entry = Table + (SIZE_T)Middle * Info->GuardStride;
        ULONG EntryRva;

        RtlCopyMemory(&EntryRva, Entry, sizeof(EntryRva));
        if (EntryRva < TargetRva)
            Low = Middle + 1;
        else if (EntryRva > TargetRva)
            High = Middle;
        else
        {
            if (Info->GuardStride > sizeof(ULONG) &&
                (Entry[sizeof(ULONG)] & IMAGE_GUARD_FLAG_FID_SUPPRESSED))
            {
                return FALSE;
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOLEAN
NTAPI
LdrpValidateUserCallTarget(
    _In_ PVOID Target)
{
    LDRP_GUARD_MODULE_INFO CachedInfo;
    PLIST_ENTRY ListHead, NextEntry;
    PPEB Peb;

    Peb = NtCurrentPeb();
    if (Peb->Ldr == NULL)
        return FALSE;

    if (LdrpGuardReadCachedModule((ULONG_PTR)Target, &CachedInfo))
        return LdrpGuardEvaluateCached((ULONG_PTR)Target, &CachedInfo);

    ListHead = &Peb->Ldr->InLoadOrderModuleList;
    for (NextEntry = ListHead->Flink; NextEntry != ListHead; NextEntry = NextEntry->Flink)
    {
        PLDR_DATA_TABLE_ENTRY LdrEntry;

        LdrEntry = CONTAINING_RECORD(NextEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        if (!LdrpGuardAddressInImage((ULONG_PTR)Target, LdrEntry, 1))
            continue;

        if (!LdrpGuardDescribeModule(LdrEntry, &CachedInfo))
            return FALSE;

        LdrpGuardPublishCachedModule(&CachedInfo);
        return LdrpGuardEvaluateCached((ULONG_PTR)Target, &CachedInfo);
    }

    return LdrpGuardDynamicTargetIsExecutable(Target);
}

NTSTATUS
NTAPI
LdrpInitializeGuard(
    _In_ PLDR_DATA_TABLE_ENTRY LdrEntry)
{
    PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig = NULL;
    PIMAGE_NT_HEADERS NtHeaders;
    PVOID *CheckSlot, *DispatchSlot;
    PVOID CheckTarget, DispatchTarget;
    PVOID ProtectBase;
    SIZE_T ProtectSize;
    ULONG ConfigSize = 0;
    ULONG OldProtection, IgnoredProtection;
    NTSTATUS Status;
    BOOLEAN GuardImage;

    NtHeaders = RtlImageNtHeader(LdrEntry->DllBase);
    if (NtHeaders != NULL)
        LoadConfig = LdrpGuardGetLoadConfig(LdrEntry, NtHeaders, &ConfigSize);
    GuardImage = LoadConfig != NULL &&
                 (NtHeaders->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF) != 0 &&
                 ConfigSize >= RTL_SIZEOF_THROUGH_FIELD(IMAGE_LOAD_CONFIG_DIRECTORY, GuardFlags) &&
                 (LoadConfig->GuardFlags & IMAGE_GUARD_CF_INSTRUMENTED) != 0;
    if (LdrEntry == LdrpImageEntry)
    {
        LdrpGuardEnabled = GuardImage;
        LdrpGuardBypass = NtHeaders != NULL &&
                          NtHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_NATIVE;
    }

    /* Native ARM64 leaves image-local thunks intact in a non-CFG process. */
    if (!LdrpGuardEnabled || !GuardImage)
        return STATUS_SUCCESS;

    if (LdrpGuardBypass)
        return STATUS_SUCCESS;

    if (!LdrpArchGetGuardTargets(&CheckTarget, &DispatchTarget))
        return STATUS_SUCCESS;

    if (ConfigSize < RTL_SIZEOF_THROUGH_FIELD(IMAGE_LOAD_CONFIG_DIRECTORY, GuardCFDispatchFunctionPointer) ||
        LoadConfig->GuardCFCheckFunctionPointer == 0)
    {
        return STATUS_INVALID_IMAGE_FORMAT;
    }

    if (LoadConfig->GuardCFDispatchFunctionPointer == 0)
        DispatchTarget = NULL;

    CheckSlot = (PVOID *)(ULONG_PTR)LoadConfig->GuardCFCheckFunctionPointer;
    DispatchSlot = DispatchTarget != NULL ? (PVOID *)(ULONG_PTR)LoadConfig->GuardCFDispatchFunctionPointer : CheckSlot;
    if (!LdrpGuardAddressInImage((ULONG_PTR)CheckSlot, LdrEntry, sizeof(*CheckSlot)) ||
        !LdrpGuardAddressInImage((ULONG_PTR)DispatchSlot, LdrEntry, sizeof(*DispatchSlot)))
    {
        return STATUS_INVALID_IMAGE_FORMAT;
    }

    ProtectBase = CheckSlot < DispatchSlot ? (PVOID)CheckSlot : (PVOID)DispatchSlot;
    ProtectSize = ((ULONG_PTR)(CheckSlot < DispatchSlot ? DispatchSlot : CheckSlot) + sizeof(PVOID)) - (ULONG_PTR)ProtectBase;
    Status = NtProtectVirtualMemory(NtCurrentProcess(), &ProtectBase, &ProtectSize, PAGE_READWRITE, &OldProtection);
    if (!NT_SUCCESS(Status))
        return Status;

    InterlockedExchangePointer((PVOID volatile *)CheckSlot, CheckTarget);
    if (DispatchTarget != NULL)
        InterlockedExchangePointer((PVOID volatile *)DispatchSlot, DispatchTarget);

    Status = NtProtectVirtualMemory(NtCurrentProcess(), &ProtectBase, &ProtectSize, OldProtection, &IgnoredProtection);
    return Status;
}
