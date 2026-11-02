/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test list for the mscoree API tests
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#define STANDALONE
#include <apitest.h>

extern void func_NetFrameworkSetup(void);

const struct test winetest_testlist[] =
{
    { "NetFrameworkSetup", func_NetFrameworkSetup },
    { 0, 0 }
};
