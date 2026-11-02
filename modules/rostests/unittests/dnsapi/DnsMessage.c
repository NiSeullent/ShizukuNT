/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Unit tests for the dnsapi DNS message parser
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>

#include <windns.h>

#include "message.h"

#define TEST_ID 0x1234
#define ANSWERS 29

#define HEADER(Flags, Answers) 0x12, 0x34, 0x81, (Flags), 0x00, 0x01, 0x00, (Answers), 0x00, 0x00, 0x00, 0x00
#define QUESTION(Type) 0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 0x03, 'c', 'o', 'm', 0x00, 0x00, (Type), 0x00, 0x01
#define RR(Type, Length) 0xC0, 0x0C, 0x00, (Type), 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, (Length)

static const UCHAR ReplyA[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_A, 4), 192, 0, 2, 1
};

static const UCHAR ReplyAaaa[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_AAAA),
    RR(DNS_TYPE_AAAA, 16), 0x20, 0x01, 0x0D, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01
};

static const UCHAR ReplyCname[] =
{
    HEADER(0x80, 2), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_CNAME, 4), 0x01, 'a', 0xC0, 0x0C,
    0xC0, 0x29, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x04, 198, 51, 100, 7
};

static const UCHAR ReplyMx[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_MX),
    RR(DNS_TYPE_MX, 9), 0x00, 0x0A, 0x04, 'm', 'a', 'i', 'l', 0xC0, 0x0C
};

static const UCHAR ReplySrv[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_SRV),
    RR(DNS_TYPE_SRV, 12), 0x00, 0x01, 0x00, 0x02, 0x01, 0xBB, 0x03, 's', 'r', 'v', 0xC0, 0x0C
};

static const UCHAR ReplySoa[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_SOA),
    RR(DNS_TYPE_SOA, 33), 0x02, 'n', 's', 0xC0, 0x0C, 0x05, 'a', 'd', 'm', 'i', 'n', 0xC0, 0x0C,
    0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x0E, 0x10, 0x00, 0x00, 0x02, 0x58, 0x00, 0x09, 0x3A, 0x80,
    0x00, 0x00, 0x01, 0x2C
};

static const UCHAR ReplyTxt[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_TEXT),
    RR(DNS_TYPE_TEXT, 9), 0x02, 'z', 'y', 0x00, 0x04, 'X', 'Y', 0xC3, 0xA9
};

static const UCHAR ReplyOtherType[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_AAAA, 16), 0x20, 0x01, 0x0D, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01
};

static const UCHAR ReplyHinfo[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_HINFO),
    RR(DNS_TYPE_HINFO, 7), 0x03, 'C', 'P', 'U', 0x02, 'O', 'S'
};

static const UCHAR QuestionLoop[] =
{
    HEADER(0x80, 1), 0xC0, 0x0C, 0x00, DNS_TYPE_A, 0x00, 0x01,
    RR(DNS_TYPE_A, 4), 192, 0, 2, 1
};

static const UCHAR AnswerLoop[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0xC0, 0x1F, 0xC0, 0x1D, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1
};

static const UCHAR AnswerSelfLoop[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0xC0, 0x1D, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1
};

static const UCHAR PointerOutside[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0xC0, 0xFF, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1
};

static const UCHAR PointerCut[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A), 0xC0
};

static const UCHAR CnameLoop[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_CNAME, 2), 0xC0, 0x29
};

static const UCHAR ExtendedLabel[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0x41, 0x00, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1
};

static const UCHAR ReservedLabel[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0x80, 0x00, 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1
};

static const UCHAR LabelOverrun[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    0x3F, 'a', 'b', 'c'
};

static const UCHAR MissingAnswer[] =
{
    HEADER(0x80, 2), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_A, 4), 192, 0, 2, 1
};

static const UCHAR RdataOverrun[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_A, 8), 192, 0, 2, 1
};

static const UCHAR BadALength[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_A, 5), 192, 0, 2, 1, 0
};

static const UCHAR BadAaaaLength[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_AAAA),
    RR(DNS_TYPE_AAAA, 4), 192, 0, 2, 1
};

static const UCHAR EmptyCname[] =
{
    HEADER(0x80, 2), QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_CNAME, 0),
    RR(DNS_TYPE_A, 4), 192, 0, 2, 1
};

static const UCHAR MxNameOutside[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_MX),
    RR(DNS_TYPE_MX, 4), 0x00, 0x0A, 0x01, 'a', 0x00
};

static const UCHAR ShortSrv[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_SRV),
    RR(DNS_TYPE_SRV, 6), 0x00, 0x01, 0x00, 0x02, 0x01, 0xBB
};

static const UCHAR ShortSoa[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_SOA),
    RR(DNS_TYPE_SOA, 29), 0x02, 'n', 's', 0xC0, 0x0C, 0x05, 'a', 'd', 'm', 'i', 'n', 0xC0, 0x0C,
    0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x0E, 0x10, 0x00, 0x00, 0x02, 0x58, 0x00, 0x09, 0x3A, 0x80
};

static const UCHAR TxtOverrun[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_TEXT),
    RR(DNS_TYPE_TEXT, 3), 0x05, 'a', 'b'
};

static const UCHAR EmptyTxt[] =
{
    HEADER(0x80, 1), QUESTION(DNS_TYPE_TEXT),
    RR(DNS_TYPE_TEXT, 0)
};

static const UCHAR Truncated[] =
{
    0x12, 0x34, 0x83, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, QUESTION(DNS_TYPE_A),
    RR(DNS_TYPE_A, 4), 192, 0
};

void
DnsIntFreeRecordList(PDNS_RECORD ToFree)
{
    PDNS_RECORDW Record = (PDNS_RECORDW)ToFree;

    while (Record)
    {
        PDNS_RECORDW Next = Record->pNext;
        DWORD i;

        HeapFree(GetProcessHeap(), 0, Record->pName);
        switch (Record->wType)
        {
        case DNS_TYPE_CNAME:
        case DNS_TYPE_PTR:
        case DNS_TYPE_NS:
            HeapFree(GetProcessHeap(), 0, Record->Data.PTR.pNameHost);
            break;
        case DNS_TYPE_MX:
            HeapFree(GetProcessHeap(), 0, Record->Data.MX.pNameExchange);
            break;
        case DNS_TYPE_SRV:
            HeapFree(GetProcessHeap(), 0, Record->Data.SRV.pNameTarget);
            break;
        case DNS_TYPE_SOA:
            HeapFree(GetProcessHeap(), 0, Record->Data.SOA.pNamePrimaryServer);
            HeapFree(GetProcessHeap(), 0, Record->Data.SOA.pNameAdministrator);
            break;
        case DNS_TYPE_TEXT:
            for (i = 0; i < Record->Data.TXT.dwStringCount; i++)
                HeapFree(GetProcessHeap(), 0, Record->Data.TXT.pStringArray[i]);
            break;
        }
        HeapFree(GetProcessHeap(), 0, Record);
        Record = Next;
    }
}

static
VOID
ParseGuarded(const UCHAR *Data, INT Length, PCWSTR Name, WORD Type, PRESOLVER_REPLY Reply)
{
    SYSTEM_INFO Info;
    SIZE_T Size;
    PUCHAR Block, Packet;

    GetSystemInfo(&Info);
    Size = ((SIZE_T)Length + Info.dwPageSize - 1) & ~((SIZE_T)Info.dwPageSize - 1);
    if (!Size)
        Size = Info.dwPageSize;
    Block = VirtualAlloc(NULL, Size + Info.dwPageSize, MEM_RESERVE, PAGE_NOACCESS);
    ok(Block != NULL, "VirtualAlloc reserve failed with %lu\n", GetLastError());
    if (!Block)
        return;
    ok(VirtualAlloc(Block, Size, MEM_COMMIT, PAGE_READWRITE) != NULL, "VirtualAlloc commit failed with %lu\n", GetLastError());
    Packet = Block + Size - Length;
    CopyMemory(Packet, Data, Length);
    ResolverParseReply(Packet, Length, TEST_ID, Name, Type, Reply);
    VirtualFree(Block, 0, MEM_RELEASE);
}

#define ParseIgnored(Data, Length, Name, Type) ParseIgnored_(__LINE__, Data, Length, Name, Type)
static
VOID
ParseIgnored_(INT Line, const UCHAR *Data, INT Length, PCWSTR Name, WORD Type)
{
    RESOLVER_REPLY Reply;

    ParseGuarded(Data, Length, Name, Type, &Reply);
    ok_(__FILE__, Line)(Reply.Status == ERROR_SUCCESS, "Status = %lu, length %d\n", Reply.Status, Length);
    ok_(__FILE__, Line)(!Reply.Final, "Final = %d, length %d\n", Reply.Final, Length);
    ok_(__FILE__, Line)(Reply.Records == NULL, "Records = %p, length %d\n", Reply.Records, Length);
    DnsIntFreeRecordList((PDNS_RECORD)Reply.Records);
}

#define ParseBad(Data, Length, Name, Type) ParseBad_(__LINE__, Data, Length, Name, Type)
static
VOID
ParseBad_(INT Line, const UCHAR *Data, INT Length, PCWSTR Name, WORD Type)
{
    RESOLVER_REPLY Reply;

    ParseGuarded(Data, Length, Name, Type, &Reply);
    ok_(__FILE__, Line)(Reply.Status == DNS_ERROR_BAD_PACKET, "Status = %lu, length %d\n", Reply.Status, Length);
    ok_(__FILE__, Line)(!Reply.Final, "Final = %d, length %d\n", Reply.Final, Length);
    ok_(__FILE__, Line)(Reply.Records == NULL, "Records = %p, length %d\n", Reply.Records, Length);
    DnsIntFreeRecordList((PDNS_RECORD)Reply.Records);
}

#define ParseFinal(Data, Length, Name, Type, Status) ParseFinal_(__LINE__, Data, Length, Name, Type, Status)
static
VOID
ParseFinal_(INT Line, const UCHAR *Data, INT Length, PCWSTR Name, WORD Type, DNS_STATUS Status)
{
    RESOLVER_REPLY Reply;

    ParseGuarded(Data, Length, Name, Type, &Reply);
    ok_(__FILE__, Line)(Reply.Status == Status, "Status = %lu, expected %lu\n", Reply.Status, Status);
    ok_(__FILE__, Line)(Reply.Final, "Final = %d\n", Reply.Final);
    ok_(__FILE__, Line)(Reply.Records == NULL, "Records = %p\n", Reply.Records);
    DnsIntFreeRecordList((PDNS_RECORD)Reply.Records);
}

static
PDNS_RECORDW
ParseSuccess(const UCHAR *Data, INT Length, PCWSTR Name, WORD Type)
{
    RESOLVER_REPLY Reply;

    ParseGuarded(Data, Length, Name, Type, &Reply);
    ok(Reply.Status == ERROR_SUCCESS, "Status = %lu\n", Reply.Status);
    ok(Reply.Final, "Final = %d\n", Reply.Final);
    ok(Reply.Records != NULL, "Records = NULL\n");
    return Reply.Records;
}

static
VOID
TestReadName(VOID)
{
    static const UCHAR Names[] = { 0x00, 0x01, 'a', 0x00, 0x01, 'b', 0xC0, 0x01, 0xC0, 0x08 };
    WCHAR Name[DNS_MAX_NAME_BUFFER_LENGTH];
    INT Next;

    Next = ResolverReadName(Names, sizeof(Names), 0, Name, ARRAYSIZE(Name));
    ok(Next == 1, "Next = %d\n", Next);
    ok(!wcscmp(Name, L""), "Name = %s\n", wine_dbgstr_w(Name));

    Next = ResolverReadName(Names, sizeof(Names), 1, Name, ARRAYSIZE(Name));
    ok(Next == 4, "Next = %d\n", Next);
    ok(!wcscmp(Name, L"a"), "Name = %s\n", wine_dbgstr_w(Name));

    Next = ResolverReadName(Names, sizeof(Names), 4, Name, ARRAYSIZE(Name));
    ok(Next == 8, "Next = %d\n", Next);
    ok(!wcscmp(Name, L"b.a"), "Name = %s\n", wine_dbgstr_w(Name));

    Next = ResolverReadName(Names, sizeof(Names), 4, Name, 3);
    ok(Next == -1, "Next = %d\n", Next);

    Next = ResolverReadName(Names, sizeof(Names), 8, Name, ARRAYSIZE(Name));
    ok(Next == -1, "Next = %d\n", Next);

    Next = ResolverReadName(Names, sizeof(Names), sizeof(Names), Name, ARRAYSIZE(Name));
    ok(Next == -1, "Next = %d\n", Next);
}

static
VOID
TestBuildQuery(VOID)
{
    static const UCHAR Expected[] =
    {
        0x12, 0x34, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, QUESTION(DNS_TYPE_A)
    };
    WCHAR Long[80];
    UCHAR Query[512];
    INT Length, i;

    Length = ResolverBuildQuery(L"example.com", DNS_TYPE_A, TEST_ID, TRUE, Query, sizeof(Query));
    ok(Length == sizeof(Expected), "Length = %d\n", Length);
    ok(Length == sizeof(Expected) && !memcmp(Query, Expected, sizeof(Expected)), "Query differs\n");

    Length = ResolverBuildQuery(L"example.com.", DNS_TYPE_A, TEST_ID, TRUE, Query, sizeof(Query));
    ok(Length == sizeof(Expected), "Length = %d\n", Length);
    ok(Length == sizeof(Expected) && !memcmp(Query, Expected, sizeof(Expected)), "Query differs\n");

    Length = ResolverBuildQuery(L"example.com", DNS_TYPE_A, TEST_ID, FALSE, Query, sizeof(Query));
    ok(Length == sizeof(Expected), "Length = %d\n", Length);
    ok(Query[2] == 0x00, "Flags = 0x%02x\n", Query[2]);

    Length = ResolverBuildQuery(L"a..b", DNS_TYPE_A, TEST_ID, TRUE, Query, sizeof(Query));
    ok(Length == -1, "Length = %d\n", Length);

    for (i = 0; i < 64; i++)
        Long[i] = L'a';
    Long[64] = UNICODE_NULL;
    Length = ResolverBuildQuery(Long, DNS_TYPE_A, TEST_ID, TRUE, Query, sizeof(Query));
    ok(Length == -1, "Length = %d\n", Length);

    Length = ResolverBuildQuery(L"example.com", DNS_TYPE_A, TEST_ID, TRUE, Query, 20);
    ok(Length == -1, "Length = %d\n", Length);
}

static
VOID
TestValidReplies(VOID)
{
    static const UCHAR Ip4[] = { 192, 0, 2, 1 };
    static const UCHAR Ip4b[] = { 198, 51, 100, 7 };
    static const UCHAR Ip6[] = { 0x20, 0x01, 0x0D, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01 };
    PDNS_RECORDW Records, Record;

    Records = ParseSuccess(ReplyA, sizeof(ReplyA), L"example.com", DNS_TYPE_A);
    if (Records)
    {
        ok(!wcscmp(Records->pName, L"example.com"), "pName = %s\n", wine_dbgstr_w(Records->pName));
        ok(Records->wType == DNS_TYPE_A, "wType = %u\n", Records->wType);
        ok(Records->wDataLength == sizeof(DNS_A_DATA), "wDataLength = %u\n", Records->wDataLength);
        ok(Records->dwTtl == 300, "dwTtl = %lu\n", Records->dwTtl);
        ok(Records->Flags.S.Section == DnsSectionAnswer, "Section = %u\n", Records->Flags.S.Section);
        ok(Records->Flags.S.CharSet == DnsCharSetUnicode, "CharSet = %u\n", Records->Flags.S.CharSet);
        ok(!memcmp(&Records->Data.A.IpAddress, Ip4, sizeof(Ip4)), "IpAddress = 0x%08lx\n", Records->Data.A.IpAddress);
        ok(Records->pNext == NULL, "pNext = %p\n", Records->pNext);
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplyA, sizeof(ReplyA), L"EXAMPLE.com", DNS_TYPE_A);
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplyAaaa, sizeof(ReplyAaaa), L"example.com", DNS_TYPE_AAAA);
    if (Records)
    {
        ok(Records->wType == DNS_TYPE_AAAA, "wType = %u\n", Records->wType);
        ok(Records->wDataLength == sizeof(DNS_AAAA_DATA), "wDataLength = %u\n", Records->wDataLength);
        ok(!memcmp(&Records->Data.AAAA.Ip6Address, Ip6, sizeof(Ip6)), "Ip6Address differs\n");
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplyCname, sizeof(ReplyCname), L"example.com", DNS_TYPE_A);
    Record = Records;
    if (Record)
    {
        ok(Record->wType == DNS_TYPE_CNAME, "wType = %u\n", Record->wType);
        ok(!wcscmp(Record->pName, L"example.com"), "pName = %s\n", wine_dbgstr_w(Record->pName));
        ok(!wcscmp(Record->Data.CNAME.pNameHost, L"a.example.com"), "pNameHost = %s\n",
           wine_dbgstr_w(Record->Data.CNAME.pNameHost));
        ok(Record->wDataLength == sizeof(DNS_PTR_DATAW), "wDataLength = %u\n", Record->wDataLength);
        Record = Record->pNext;
        ok(Record != NULL, "second record missing\n");
    }
    if (Record)
    {
        ok(Record->wType == DNS_TYPE_A, "wType = %u\n", Record->wType);
        ok(!wcscmp(Record->pName, L"a.example.com"), "pName = %s\n", wine_dbgstr_w(Record->pName));
        ok(Record->dwTtl == 60, "dwTtl = %lu\n", Record->dwTtl);
        ok(!memcmp(&Record->Data.A.IpAddress, Ip4b, sizeof(Ip4b)), "IpAddress = 0x%08lx\n", Record->Data.A.IpAddress);
        ok(Record->pNext == NULL, "pNext = %p\n", Record->pNext);
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplyMx, sizeof(ReplyMx), L"example.com", DNS_TYPE_MX);
    if (Records)
    {
        ok(Records->Data.MX.wPreference == 10, "wPreference = %u\n", Records->Data.MX.wPreference);
        ok(!wcscmp(Records->Data.MX.pNameExchange, L"mail.example.com"), "pNameExchange = %s\n",
           wine_dbgstr_w(Records->Data.MX.pNameExchange));
        ok(Records->wDataLength == sizeof(DNS_MX_DATAW), "wDataLength = %u\n", Records->wDataLength);
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplySrv, sizeof(ReplySrv), L"example.com", DNS_TYPE_SRV);
    if (Records)
    {
        ok(Records->Data.SRV.wPriority == 1, "wPriority = %u\n", Records->Data.SRV.wPriority);
        ok(Records->Data.SRV.wWeight == 2, "wWeight = %u\n", Records->Data.SRV.wWeight);
        ok(Records->Data.SRV.wPort == 443, "wPort = %u\n", Records->Data.SRV.wPort);
        ok(!wcscmp(Records->Data.SRV.pNameTarget, L"srv.example.com"), "pNameTarget = %s\n",
           wine_dbgstr_w(Records->Data.SRV.pNameTarget));
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplySoa, sizeof(ReplySoa), L"example.com", DNS_TYPE_SOA);
    if (Records)
    {
        ok(!wcscmp(Records->Data.SOA.pNamePrimaryServer, L"ns.example.com"), "pNamePrimaryServer = %s\n",
           wine_dbgstr_w(Records->Data.SOA.pNamePrimaryServer));
        ok(!wcscmp(Records->Data.SOA.pNameAdministrator, L"admin.example.com"), "pNameAdministrator = %s\n",
           wine_dbgstr_w(Records->Data.SOA.pNameAdministrator));
        ok(Records->Data.SOA.dwSerialNo == 0x01020304, "dwSerialNo = 0x%lx\n", Records->Data.SOA.dwSerialNo);
        ok(Records->Data.SOA.dwRefresh == 3600, "dwRefresh = %lu\n", Records->Data.SOA.dwRefresh);
        ok(Records->Data.SOA.dwRetry == 600, "dwRetry = %lu\n", Records->Data.SOA.dwRetry);
        ok(Records->Data.SOA.dwExpire == 604800, "dwExpire = %lu\n", Records->Data.SOA.dwExpire);
        ok(Records->Data.SOA.dwDefaultTtl == 300, "dwDefaultTtl = %lu\n", Records->Data.SOA.dwDefaultTtl);
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Records = ParseSuccess(ReplyTxt, sizeof(ReplyTxt), L"example.com", DNS_TYPE_TEXT);
    if (Records)
    {
        ok(Records->Data.TXT.dwStringCount == 3, "dwStringCount = %lu\n", Records->Data.TXT.dwStringCount);
        ok(Records->wDataLength == FIELD_OFFSET(DNS_TXT_DATAW, pStringArray[3]), "wDataLength = %u\n",
           Records->wDataLength);
        if (Records->Data.TXT.dwStringCount == 3)
        {
            ok(!wcscmp(Records->Data.TXT.pStringArray[0], L"zy"), "String 0 = %s\n",
               wine_dbgstr_w(Records->Data.TXT.pStringArray[0]));
            ok(!wcscmp(Records->Data.TXT.pStringArray[1], L""), "String 1 = %s\n",
               wine_dbgstr_w(Records->Data.TXT.pStringArray[1]));
            ok(!wcscmp(Records->Data.TXT.pStringArray[2], L"XY\x00e9"), "String 2 = %s\n",
               wine_dbgstr_w(Records->Data.TXT.pStringArray[2]));
        }
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);
}

static
VOID
TestStatusReplies(VOID)
{
    static const UCHAR NameError[] = { HEADER(0x83, 0), QUESTION(DNS_TYPE_A) };
    static const UCHAR FormatError[] = { HEADER(0x81, 0), QUESTION(DNS_TYPE_A) };
    static const UCHAR ServerFailure[] = { HEADER(0x82, 0), QUESTION(DNS_TYPE_A) };
    static const UCHAR Refused[] = { HEADER(0x85, 0), QUESTION(DNS_TYPE_A) };
    UCHAR Copy[sizeof(ReplyA)];
    RESOLVER_REPLY Reply;

    ParseFinal(NameError, sizeof(NameError), L"example.com", DNS_TYPE_A, DNS_ERROR_RCODE_NAME_ERROR);
    ParseFinal(ReplyOtherType, sizeof(ReplyOtherType), L"example.com", DNS_TYPE_A, DNS_INFO_NO_RECORDS);
    ParseFinal(ReplyHinfo, sizeof(ReplyHinfo), L"example.com", DNS_TYPE_HINFO, DNS_INFO_NO_RECORDS);

    ParseGuarded(FormatError, sizeof(FormatError), L"example.com", DNS_TYPE_A, &Reply);
    ok(Reply.Status == DNS_ERROR_RCODE_FORMAT_ERROR && !Reply.Final && !Reply.Records, "Status = %lu\n", Reply.Status);
    ParseGuarded(ServerFailure, sizeof(ServerFailure), L"example.com", DNS_TYPE_A, &Reply);
    ok(Reply.Status == DNS_ERROR_RCODE_SERVER_FAILURE && !Reply.Final && !Reply.Records, "Status = %lu\n", Reply.Status);
    ParseGuarded(Refused, sizeof(Refused), L"example.com", DNS_TYPE_A, &Reply);
    ok(Reply.Status == DNS_ERROR_RCODE_REFUSED && !Reply.Final && !Reply.Records, "Status = %lu\n", Reply.Status);

    ParseIgnored(ReplyA, sizeof(ReplyA), L"example.org", DNS_TYPE_A);
    ParseIgnored(ReplyA, sizeof(ReplyA), L"example.com", DNS_TYPE_AAAA);

    CopyMemory(Copy, ReplyA, sizeof(Copy));
    Copy[0] = 0x43;
    ParseIgnored(Copy, sizeof(Copy), L"example.com", DNS_TYPE_A);

    CopyMemory(Copy, ReplyA, sizeof(Copy));
    Copy[2] = 0x01;
    ParseIgnored(Copy, sizeof(Copy), L"example.com", DNS_TYPE_A);
}

static
VOID
TestCompression(VOID)
{
    ParseIgnored(QuestionLoop, sizeof(QuestionLoop), L"example.com", DNS_TYPE_A);
    ParseBad(AnswerLoop, sizeof(AnswerLoop), L"example.com", DNS_TYPE_A);
    ParseBad(AnswerSelfLoop, sizeof(AnswerSelfLoop), L"example.com", DNS_TYPE_A);
    ParseBad(PointerOutside, sizeof(PointerOutside), L"example.com", DNS_TYPE_A);
    ParseBad(PointerCut, sizeof(PointerCut), L"example.com", DNS_TYPE_A);
    ParseBad(CnameLoop, sizeof(CnameLoop), L"example.com", DNS_TYPE_A);
    ParseBad(ExtendedLabel, sizeof(ExtendedLabel), L"example.com", DNS_TYPE_A);
    ParseBad(ReservedLabel, sizeof(ReservedLabel), L"example.com", DNS_TYPE_A);
}

static
VOID
TestMalformed(VOID)
{
    INT Length;

    ParseBad(LabelOverrun, sizeof(LabelOverrun), L"example.com", DNS_TYPE_A);
    ParseBad(MissingAnswer, sizeof(MissingAnswer), L"example.com", DNS_TYPE_A);
    ParseBad(RdataOverrun, sizeof(RdataOverrun), L"example.com", DNS_TYPE_A);
    ParseBad(BadALength, sizeof(BadALength), L"example.com", DNS_TYPE_A);
    ParseBad(BadAaaaLength, sizeof(BadAaaaLength), L"example.com", DNS_TYPE_AAAA);
    ParseBad(EmptyCname, sizeof(EmptyCname), L"example.com", DNS_TYPE_A);
    ParseBad(MxNameOutside, sizeof(MxNameOutside), L"example.com", DNS_TYPE_MX);
    ParseBad(ShortSrv, sizeof(ShortSrv), L"example.com", DNS_TYPE_SRV);
    ParseBad(ShortSoa, sizeof(ShortSoa), L"example.com", DNS_TYPE_SOA);
    ParseBad(TxtOverrun, sizeof(TxtOverrun), L"example.com", DNS_TYPE_TEXT);
    ParseBad(EmptyTxt, sizeof(EmptyTxt), L"example.com", DNS_TYPE_TEXT);
    ParseBad(Truncated, sizeof(Truncated), L"example.com", DNS_TYPE_A);

    for (Length = 0; Length < (INT)sizeof(ReplyCname); Length++)
    {
        if (Length < ANSWERS)
            ParseIgnored(ReplyCname, Length, L"example.com", DNS_TYPE_A);
        else
            ParseBad(ReplyCname, Length, L"example.com", DNS_TYPE_A);
    }

    for (Length = 0; Length < (INT)sizeof(ReplySoa); Length++)
    {
        if (Length < ANSWERS)
            ParseIgnored(ReplySoa, Length, L"example.com", DNS_TYPE_SOA);
        else
            ParseBad(ReplySoa, Length, L"example.com", DNS_TYPE_SOA);
    }
}

static
INT
BuildOwnerReply(PUCHAR Packet, const UCHAR *Labels, ULONG Count)
{
    static const UCHAR Head[] = { HEADER(0x80, 1), QUESTION(DNS_TYPE_A) };
    static const UCHAR Tail[] = { 0x00, DNS_TYPE_A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x04, 192, 0, 2, 1 };
    INT Offset = sizeof(Head);
    ULONG i;

    CopyMemory(Packet, Head, sizeof(Head));
    for (i = 0; i < Count; i++)
    {
        Packet[Offset++] = Labels[i];
        FillMemory(Packet + Offset, Labels[i], 'a');
        Offset += Labels[i];
    }
    Packet[Offset++] = 0;
    CopyMemory(Packet + Offset, Tail, sizeof(Tail));
    return Offset + sizeof(Tail);
}

static
VOID
TestNameLength(VOID)
{
    static const UCHAR Longest[] = { 63, 63, 63, 61 };
    static const UCHAR TooLong[] = { 63, 63, 63, 62 };
    UCHAR Packet[512];
    PDNS_RECORDW Records;
    INT Length;

    Length = BuildOwnerReply(Packet, Longest, ARRAYSIZE(Longest));
    Records = ParseSuccess(Packet, Length, L"example.com", DNS_TYPE_A);
    if (Records)
        ok(wcslen(Records->pName) == DNS_MAX_NAME_LENGTH - 2, "pName length = %u\n", (UINT)wcslen(Records->pName));
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Length = BuildOwnerReply(Packet, TooLong, ARRAYSIZE(TooLong));
    ParseBad(Packet, Length, L"example.com", DNS_TYPE_A);
}

static
INT
BuildTxtReply(PUCHAR Packet, DWORD Strings)
{
    static const UCHAR Head[] =
    {
        HEADER(0x80, 1), QUESTION(DNS_TYPE_TEXT),
        0xC0, 0x0C, 0x00, DNS_TYPE_TEXT, 0x00, 0x01, 0x00, 0x00, 0x01, 0x2C
    };

    CopyMemory(Packet, Head, sizeof(Head));
    Packet[sizeof(Head)] = (UCHAR)(Strings >> 8);
    Packet[sizeof(Head) + 1] = (UCHAR)Strings;
    ZeroMemory(Packet + sizeof(Head) + 2, Strings);
    return sizeof(Head) + 2 + Strings;
}

static
VOID
TestTxtDataLength(VOID)
{
    DWORD MaxStrings = (MAXWORD - FIELD_OFFSET(DNS_TXT_DATAW, pStringArray)) / sizeof(PWSTR);
    PDNS_RECORDW Records;
    PUCHAR Packet;
    INT Length;

    Packet = HeapAlloc(GetProcessHeap(), 0, 64 + MAXWORD);
    ok(Packet != NULL, "HeapAlloc failed\n");
    if (!Packet)
        return;

    Length = BuildTxtReply(Packet, MaxStrings);
    Records = ParseSuccess(Packet, Length, L"example.com", DNS_TYPE_TEXT);
    if (Records)
    {
        ok(Records->Data.TXT.dwStringCount == MaxStrings, "dwStringCount = %lu\n", Records->Data.TXT.dwStringCount);
        ok(Records->wDataLength == FIELD_OFFSET(DNS_TXT_DATAW, pStringArray) + MaxStrings * sizeof(PWSTR),
           "wDataLength = %u\n", Records->wDataLength);
    }
    DnsIntFreeRecordList((PDNS_RECORD)Records);

    Length = BuildTxtReply(Packet, MaxStrings + 1);
    ParseBad(Packet, Length, L"example.com", DNS_TYPE_TEXT);

    HeapFree(GetProcessHeap(), 0, Packet);
}

START_TEST(DnsMessage)
{
    TestReadName();
    TestBuildQuery();
    TestValidReplies();
    TestStatusReplies();
    TestCompression();
    TestMalformed();
    TestNameLength();
    TestTxtDataLength();
}
