/*
 * PROJECT:     LiberNT API Tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the C++ frame handler 4 and the debugger hook exports of ucrtbase
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>

#include <windows.h>
#include <ntstatus.h>
#include <string.h>

typedef void (__cdecl *PDEBUGGER_HOOK)(int);
typedef void (__cdecl *PSET_WINRT_OOM_CALLBACK)(void *);

#if defined(_M_AMD64) && !defined(_M_ARM64EC)

typedef EXCEPTION_DISPOSITION (__cdecl *PCXX_FRAME_HANDLER4)(EXCEPTION_RECORD *, ULONG64, CONTEXT *, DISPATCHER_CONTEXT *);

#define FH4_HEADER_UNWIND_MAP 0x08
#define FH4_HEADER_EHS        0x20
#define FH4_UNWIND_DTOR_OBJ   1

static BYTE Fh4Data[64];
static ULONG64 DestructorObject;
static ULONG64 DestructorFrame;
static LONG DestructorCalls;

static void __cdecl UnwindDestructor(void *Object, ULONG64 Frame)
{
    DestructorObject = (ULONG64)Object;
    DestructorFrame = Frame;
    DestructorCalls++;
}

static ULONG RvaOf(ULONG_PTR ImageBase, const void *Address)
{
    return (ULONG)((ULONG_PTR)Address - ImageBase);
}

static void WriteRva(BYTE *Destination, ULONG Rva)
{
    memcpy(Destination, &Rva, sizeof(Rva));
}

static void CheckFrameHandler4(HMODULE Module)
{
    PCXX_FRAME_HANDLER4 Handler = (PCXX_FRAME_HANDLER4)(PVOID)GetProcAddress(Module, "__CxxFrameHandler4");
    ULONG_PTR ImageBase = (ULONG_PTR)GetModuleHandleW(NULL);
    RUNTIME_FUNCTION Function;
    DISPATCHER_CONTEXT Dispatch;
    EXCEPTION_RECORD Record;
    ULONG64 FrameData[4];
    ULONG HandlerData;
    EXCEPTION_DISPOSITION Disposition;

    ok(Handler != NULL, "__CxxFrameHandler4 is not exported\n");
    if (!Handler) return;

    ZeroMemory(&Function, sizeof(Function));
    ZeroMemory(&Dispatch, sizeof(Dispatch));
    ZeroMemory(&Record, sizeof(Record));
    ZeroMemory(Fh4Data, sizeof(Fh4Data));
    Dispatch.ImageBase = ImageBase;
    Dispatch.ControlPc = ImageBase + 0x10;
    Dispatch.FunctionEntry = &Function;
    Dispatch.HandlerData = &HandlerData;
    Record.ExceptionCode = STATUS_ACCESS_VIOLATION;

    Fh4Data[0] = FH4_HEADER_EHS;
    HandlerData = RvaOf(ImageBase, &Fh4Data[0]);
    DestructorCalls = 0;
    Disposition = Handler(&Record, (ULONG64)FrameData, NULL, &Dispatch);
    ok(Disposition == ExceptionContinueSearch, "SEH exception through an /EHs frame returned %d\n", Disposition);
    ok(DestructorCalls == 0, "SEH exception through an /EHs frame ran %ld destructors\n", DestructorCalls);

    Fh4Data[0] = FH4_HEADER_UNWIND_MAP;
    WriteRva(&Fh4Data[1], RvaOf(ImageBase, &Fh4Data[16]));
    WriteRva(&Fh4Data[5], RvaOf(ImageBase, &Fh4Data[32]));
    Fh4Data[16] = 1 << 1;
    Fh4Data[17] = ((1 << 2) | FH4_UNWIND_DTOR_OBJ) << 1;
    WriteRva(&Fh4Data[18], RvaOf(ImageBase, (const void *)UnwindDestructor));
    Fh4Data[22] = (2 * sizeof(ULONG64)) << 1;
    Fh4Data[32] = 1 << 1;
    Fh4Data[33] = 0 << 1;
    Fh4Data[34] = 1 << 1;
    HandlerData = RvaOf(ImageBase, &Fh4Data[0]);
    Record.ExceptionFlags = EXCEPTION_UNWINDING;
    DestructorCalls = 0;
    DestructorObject = DestructorFrame = 0;
    Disposition = Handler(&Record, (ULONG64)FrameData, NULL, &Dispatch);
    ok(Disposition == ExceptionContinueSearch, "unwinding returned %d\n", Disposition);
    ok(DestructorCalls == 1, "unwinding ran %ld destructors\n", DestructorCalls);
    ok(DestructorObject == (ULONG64)&FrameData[2], "destructor object %#I64x, expected %p\n", DestructorObject, &FrameData[2]);
    ok(DestructorFrame == (ULONG64)FrameData, "destructor frame %#I64x, expected %p\n", DestructorFrame, FrameData);
}

#else

static void CheckFrameHandler4(HMODULE Module)
{
#if defined(_M_IX86)
    ok(GetProcAddress(Module, "__CxxFrameHandler4") == NULL, "__CxxFrameHandler4 is exported on x86\n");
#else
    (void)Module;
    skip("__CxxFrameHandler4 is only implemented for x64\n");
#endif
}

#endif

static void CheckDebuggerHooks(HMODULE Module)
{
    PDEBUGGER_HOOK DebuggerHook = (PDEBUGGER_HOOK)(PVOID)GetProcAddress(Module, "_crt_debugger_hook");
    PSET_WINRT_OOM_CALLBACK SetCallback = (PSET_WINRT_OOM_CALLBACK)(PVOID)GetProcAddress(Module, "_SetWinRTOutOfMemoryExceptionCallback");

    ok(DebuggerHook != NULL, "_crt_debugger_hook is not exported\n");
    ok(SetCallback != NULL, "_SetWinRTOutOfMemoryExceptionCallback is not exported\n");
    if (DebuggerHook) DebuggerHook(0);
    if (SetCallback) SetCallback(NULL);
}

START_TEST(VcruntimeExports)
{
    HMODULE Module = GetModuleHandleW(L"ucrtbase.dll");

    ok(Module != NULL, "ucrtbase.dll is not loaded\n");
    if (!Module) return;

    CheckFrameHandler4(Module);
    CheckDebuggerHooks(Module);
}
