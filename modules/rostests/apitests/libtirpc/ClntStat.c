/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the TI-RPC client status codes
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <wintirpc.h>
#include <rpc/rpc.h>
#include <apitest.h>

static const struct
{
    enum clnt_stat Status;
    int Value;
    const char *Text;
} ClntStatTable[] =
{
    { RPC_SUCCESS, 0, "RPC: Success" },
    { RPC_CANTENCODEARGS, 1, "RPC: Can't encode arguments" },
    { RPC_CANTDECODERES, 2, "RPC: Can't decode result" },
    { RPC_CANTSEND, 3, "RPC: Unable to send" },
    { RPC_CANTRECV, 4, "RPC: Unable to receive" },
    { RPC_TIMEDOUT, 5, "RPC: Timed out" },
    { RPC_VERSMISMATCH, 6, "RPC: Incompatible versions of RPC" },
    { RPC_AUTHERROR, 7, "RPC: Authentication error" },
    { RPC_PROGUNAVAIL, 8, "RPC: Program unavailable" },
    { RPC_PROGVERSMISMATCH, 9, "RPC: Program/version mismatch" },
    { RPC_PROCUNAVAIL, 10, "RPC: Procedure unavailable" },
    { RPC_CANTDECODEARGS, 11, "RPC: Server can't decode arguments" },
    { RPC_SYSTEMERROR, 12, "RPC: Remote system error" },
    { RPC_UNKNOWNHOST, 13, "RPC: Unknown host" },
    { RPC_RPCBFAILURE, 14, "RPC: Port mapper failure" },
    { RPC_PROGNOTREGISTERED, 15, "RPC: Program not registered" },
    { RPC_FAILED, 16, "RPC: Failed (unspecified error)" },
    { RPC_UNKNOWNPROTO, 17, "RPC: Unknown protocol" },
    { RPC_INTR, 18, "RPC: (unknown error code)" },
    { RPC_UNKNOWNADDR, 19, "RPC: (unknown error code)" },
    { RPC_TLIERROR, 20, "RPC: (unknown error code)" },
    { RPC_NOBROADCAST, 21, "RPC: (unknown error code)" },
    { RPC_N2AXLATEFAILURE, 22, "RPC: (unknown error code)" },
    { RPC_UDERROR, 23, "RPC: (unknown error code)" },
    { RPC_INPROGRESS, 24, "RPC: (unknown error code)" },
    { RPC_STALERACHANDLE, 25, "RPC: (unknown error code)" },
    { RPC_CANTCONNECT, 26, "RPC: (unknown error code)" },
    { RPC_XPRTFAILED, 27, "RPC: (unknown error code)" },
    { RPC_CANTCREATESTREAM, 28, "RPC: (unknown error code)" },
};

START_TEST(ClntStat)
{
    unsigned int i;
    const char *Text;

    for (i = 0; i < ARRAYSIZE(ClntStatTable); i++)
    {
        winetest_push_context("%u", i);
        ok_int((int)ClntStatTable[i].Status, ClntStatTable[i].Value);
        Text = clnt_sperrno(ClntStatTable[i].Status);
        ok(Text != NULL, "clnt_sperrno returned NULL\n");
        if (Text != NULL)
            ok_str(Text, ClntStatTable[i].Text);
        winetest_pop_context();
    }

    ok_int((int)RPC_PMAPFAILURE, (int)RPC_RPCBFAILURE);
}
