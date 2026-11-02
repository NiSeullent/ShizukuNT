/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     Debugger hook and WinRT callback registration from Wine 11.21 dlls/msvcrt/misc.c
 * COPYRIGHT:   Copyright 2000 Jon Griffiths
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

void __cdecl _crt_debugger_hook(int reserved)
{
    (void)reserved;
}

void __cdecl _SetWinRTOutOfMemoryExceptionCallback(void *callback)
{
    /* TODO: the callback is ignored */
    (void)callback;
}
