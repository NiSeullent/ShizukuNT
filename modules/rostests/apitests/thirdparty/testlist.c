/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test list for the bundled third-party library tests
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#define STANDALONE
#include <apitest.h>

extern void func_libpng(void);
extern void func_libtiff(void);
extern void func_minizip(void);
extern void func_zlib(void);

const struct test winetest_testlist[] =
{
    { "libpng", func_libpng },
    { "libtiff", func_libtiff },
    { "minizip", func_minizip },
    { "zlib", func_zlib },
    { 0, 0 }
};
