/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for svc_run() and the server descriptor lock
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <wintirpc.h>
#include <rpc/rpc.h>
#include <apitest.h>
#include <pseh/pseh2.h>

static DWORD
CallSvcRun(void)
{
    DWORD ExceptionCode = 0;

    _SEH2_TRY
    {
        svc_run();
    }
    _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        ExceptionCode = _SEH2_GetExceptionCode();
    }
    _SEH2_END;

    return ExceptionCode;
}

START_TEST(SvcRun)
{
    unsigned int i;

    for (i = 0; i < 2; i++)
    {
        winetest_push_context("call %u", i);
        WSASetLastError(0);
        ok_hex(CallSvcRun(), 0);
        ok_int(WSAGetLastError(), WSAEINVAL);
        winetest_pop_context();
    }
}
