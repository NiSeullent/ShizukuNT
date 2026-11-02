/*
 * PROJECT:     LiberNT Universal C Runtime
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Allocates and frees the __CxxFrameHandler4 try-level slot with the CRT initializers
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <internal_shared.h>

BOOL msvcrt_init_handler4(void);
void msvcrt_free_handler4(void);

static int __cdecl initialize_handler4(void)
{
    return msvcrt_init_handler4() ? 0 : 1;
}

static void __cdecl uninitialize_handler4(void)
{
    msvcrt_free_handler4();
}

_CRTALLOC(".CRT$XIC") _PIFV const __ucrtbase_handler4_initializer = initialize_handler4;
_CRTALLOC(".CRT$XPX") _PVFV const __ucrtbase_handler4_terminator = uninitialize_handler4;
