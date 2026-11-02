/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test list for the TI-RPC library tests
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#define STANDALONE
#include <apitest.h>

extern void func_ClntStat(void);
extern void func_GetTimeOfDay(void);
extern void func_SvcRun(void);

const struct test winetest_testlist[] =
{
    { "ClntStat", func_ClntStat },
    { "GetTimeOfDay", func_GetTimeOfDay },
    { "SvcRun", func_SvcRun },
    { 0, 0 }
};
