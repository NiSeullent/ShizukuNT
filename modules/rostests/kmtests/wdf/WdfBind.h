/*
 * PROJECT:     LiberNT kernel-mode tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     KMDF bind, version and timer test declarations
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#ifndef _KMTEST_WDFBIND_H_
#define _KMTEST_WDFBIND_H_

#define IOCTL_WDFBIND_RUN 1

typedef struct _WDFBIND_PARAMS
{
    ULONG MajorVersion;
    ULONG MinorVersion;
    ULONG64 FunctionTable;
} WDFBIND_PARAMS, *PWDFBIND_PARAMS;

#endif /* !defined _KMTEST_WDFBIND_H_ */
