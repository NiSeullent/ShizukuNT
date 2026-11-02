/*
 * PROJECT:     LiberNT DNS API
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     DNS message encoding and response parsing declarations
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif193@gmail.com>
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#pragma once

typedef struct _RESOLVER_REPLY
{
    DNS_STATUS Status;
    BOOL Final;
    PDNS_RECORDW Records;
} RESOLVER_REPLY, *PRESOLVER_REPLY;

INT ResolverBuildQuery(PCWSTR Name, WORD Type, WORD Id, BOOL Recursion, PUCHAR Packet, INT Capacity);
INT ResolverReadName(const UCHAR *Packet, INT Length, INT Offset, PWSTR Name, INT Count);
VOID ResolverParseReply(const UCHAR *Packet, INT Length, WORD Id, PCWSTR Name, WORD Type, PRESOLVER_REPLY Reply);
void DnsIntFreeRecordList(PDNS_RECORD ToFree);
