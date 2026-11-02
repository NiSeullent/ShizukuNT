/*
 * PROJECT:     LiberNT DNS API
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     DNS stub resolver over IPv4 and IPv6 servers
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif193@gmail.com>
 */

#include "precomp.h"

#include <winreg.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ntsecapi.h>
#include <strsafe.h>

#include "message.h"

#define NDEBUG
#include <debug.h>

#define DNS_PORT 53
#define LLMNR_PORT 5355
#define LLMNR_RESEND 420
#define LLMNR_ROUNDS 2
#define DNS_MAX_ADAPTERS 16
#define DNS_MAX_SERVERS 32
#define DNS_MAX_CANDIDATES 8
#define DNS_UDP_SIZE 1232
#define DNS_TCP_SIZE 65535
#define DNS_TCP_TIMEOUT 4000

typedef struct _RESOLVER_SERVER
{
    SOCKADDR_INET Address;
    ULONG Adapter;
    BOOL Sent;
    BOOL Failed;
} RESOLVER_SERVER, *PRESOLVER_SERVER;

typedef struct _RESOLVER_ADAPTER
{
    ULONG Metric;
    ULONG FirstServer;
    ULONG ServerCount;
    BOOL Negative;
    WCHAR Suffix[MAX_DNS_SUFFIX_STRING_LENGTH];
    WCHAR SearchList[1024];
} RESOLVER_ADAPTER, *PRESOLVER_ADAPTER;

typedef struct _RESOLVER_CONFIG
{
    RESOLVER_ADAPTER Adapters[DNS_MAX_ADAPTERS];
    ULONG AdapterCount;
    RESOLVER_SERVER Servers[DNS_MAX_SERVERS];
    ULONG ServerCount;
    WCHAR Candidates[DNS_MAX_CANDIDATES][DNS_MAX_NAME_BUFFER_LENGTH];
    ULONG CandidateCount;
} RESOLVER_CONFIG, *PRESOLVER_CONFIG;

static LONG ResolverWinsockStarted;

static
VOID
ResolverStartWinsock(VOID)
{
    WSADATA Data;

    if (InterlockedCompareExchange(&ResolverWinsockStarted, 1, 0) == 0)
        WSAStartup(MAKEWORD(2, 2), &Data);
}

static
int __cdecl
ResolverCompareAdapters(const void *Left, const void *Right)
{
    const RESOLVER_ADAPTER *A = Left, *B = Right;

    return A->Metric < B->Metric ? -1 : A->Metric > B->Metric;
}

static
VOID
ResolverAddCandidate(PRESOLVER_CONFIG Config, PCWSTR Name, PCWSTR Suffix)
{
    WCHAR Buffer[DNS_MAX_NAME_BUFFER_LENGTH];
    ULONG i;

    if (Config->CandidateCount == DNS_MAX_CANDIDATES)
        return;
    if (Suffix && *Suffix)
    {
        if (FAILED(StringCchPrintfW(Buffer, ARRAYSIZE(Buffer), L"%s.%s", Name, Suffix)))
            return;
    }
    else if (FAILED(StringCchCopyW(Buffer, ARRAYSIZE(Buffer), Name)))
    {
        return;
    }

    for (i = 0; i < Config->CandidateCount; i++)
    {
        if (!_wcsicmp(Config->Candidates[i], Buffer))
            return;
    }
    StringCchCopyW(Config->Candidates[Config->CandidateCount++], DNS_MAX_NAME_BUFFER_LENGTH, Buffer);
}

static
VOID
ResolverReadRegistryString(PCWSTR Value, PWSTR Buffer, DWORD Count)
{
    DWORD Size = (Count - 1) * sizeof(WCHAR), Type;
    HKEY Key;

    Buffer[0] = UNICODE_NULL;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",
                      0, KEY_READ, &Key) != ERROR_SUCCESS)
        return;
    if (RegQueryValueExW(Key, Value, NULL, &Type, (PBYTE)Buffer, &Size) == ERROR_SUCCESS && Type == REG_SZ)
        Buffer[Size / sizeof(WCHAR)] = UNICODE_NULL;
    else
        Buffer[0] = UNICODE_NULL;
    RegCloseKey(Key);
}

static
DNS_STATUS
ResolverLoadConfig(PRESOLVER_CONFIG Config, PCWSTR Name)
{
    ULONG Size = 16384, i;
    PIP_ADAPTER_ADDRESSES List, Adapter;
    DWORD Error;
    WCHAR Domain[MAX_DNS_SUFFIX_STRING_LENGTH], SearchList[1024];
    SIZE_T Length = wcslen(Name);
    BOOL Absolute = Length && Name[Length - 1] == L'.';
    BOOL SingleLabel = !wcschr(Name, L'.');

    RtlZeroMemory(Config, sizeof(*Config));

    for (;;)
    {
        List = RtlAllocateHeap(RtlGetProcessHeap(), 0, Size);
        if (!List)
            return ERROR_OUTOFMEMORY;
        Error = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_FRIENDLY_NAME,
                                     NULL, List, &Size);
        if (Error != ERROR_BUFFER_OVERFLOW)
            break;
        RtlFreeHeap(RtlGetProcessHeap(), 0, List);
    }
    if (Error != ERROR_SUCCESS)
    {
        RtlFreeHeap(RtlGetProcessHeap(), 0, List);
        return Error == ERROR_NO_DATA ? DNS_ERROR_NO_DNS_SERVERS : Error;
    }

    for (Adapter = List; Adapter && Config->AdapterCount < DNS_MAX_ADAPTERS; Adapter = Adapter->Next)
    {
        PIP_ADAPTER_DNS_SERVER_ADDRESS Server;
        PIP_ADAPTER_DNS_SUFFIX Suffix;
        PRESOLVER_ADAPTER Entry;

        if (Adapter->OperStatus != IfOperStatusUp || Adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK ||
            !Adapter->FirstDnsServerAddress)
            continue;

        Entry = &Config->Adapters[Config->AdapterCount];
        Entry->Metric = Adapter->Ipv4Metric;
        if (Adapter->Ipv6Metric && (!Entry->Metric || Adapter->Ipv6Metric < Entry->Metric))
            Entry->Metric = Adapter->Ipv6Metric;
        if (Adapter->DnsSuffix)
            StringCchCopyW(Entry->Suffix, ARRAYSIZE(Entry->Suffix), Adapter->DnsSuffix);
        for (Suffix = Adapter->FirstDnsSuffix; Suffix; Suffix = Suffix->Next)
        {
            if (Entry->SearchList[0])
                StringCchCatW(Entry->SearchList, ARRAYSIZE(Entry->SearchList), L",");
            StringCchCatW(Entry->SearchList, ARRAYSIZE(Entry->SearchList), Suffix->String);
        }

        Entry->FirstServer = Config->ServerCount;
        for (Server = Adapter->FirstDnsServerAddress; Server && Config->ServerCount < DNS_MAX_SERVERS; Server = Server->Next)
        {
            PRESOLVER_SERVER Slot = &Config->Servers[Config->ServerCount];
            INT Length = Server->Address.iSockaddrLength;

            if (!Server->Address.lpSockaddr || Length > (INT)sizeof(Slot->Address))
                continue;
            if (Server->Address.lpSockaddr->sa_family != AF_INET && Server->Address.lpSockaddr->sa_family != AF_INET6)
                continue;
            RtlZeroMemory(Slot, sizeof(*Slot));
            RtlCopyMemory(&Slot->Address, Server->Address.lpSockaddr, Length);
            if (Slot->Address.si_family == AF_INET6)
            {
                Slot->Address.Ipv6.sin6_port = htons(DNS_PORT);
                if (IN6_IS_ADDR_LINKLOCAL(&Slot->Address.Ipv6.sin6_addr) && !Slot->Address.Ipv6.sin6_scope_id)
                    Slot->Address.Ipv6.sin6_scope_id = Adapter->Ipv6IfIndex;
            }
            else
            {
                Slot->Address.Ipv4.sin_port = htons(DNS_PORT);
            }
            Slot->Adapter = Config->AdapterCount;
            Config->ServerCount++;
            Entry->ServerCount++;
        }
        if (Entry->ServerCount)
            Config->AdapterCount++;
    }
    RtlFreeHeap(RtlGetProcessHeap(), 0, List);

    if (!Config->AdapterCount)
        return DNS_ERROR_NO_DNS_SERVERS;

    qsort(Config->Adapters, Config->AdapterCount, sizeof(Config->Adapters[0]), ResolverCompareAdapters);
    {
        RESOLVER_SERVER Saved[DNS_MAX_SERVERS];
        ULONG j;

        RtlCopyMemory(Saved, Config->Servers, sizeof(Saved));
        Config->ServerCount = 0;
        for (i = 0; i < Config->AdapterCount; i++)
        {
            ULONG First = Config->Adapters[i].FirstServer;

            Config->Adapters[i].FirstServer = Config->ServerCount;
            for (j = 0; j < Config->Adapters[i].ServerCount; j++)
            {
                Config->Servers[Config->ServerCount] = Saved[First + j];
                Config->Servers[Config->ServerCount].Adapter = i;
                Config->ServerCount++;
            }
        }
    }

    if (Absolute)
    {
        WCHAR Trimmed[DNS_MAX_NAME_BUFFER_LENGTH];

        StringCchCopyNW(Trimmed, ARRAYSIZE(Trimmed), Name, Length - 1);
        ResolverAddCandidate(Config, Trimmed, NULL);
        return ERROR_SUCCESS;
    }

    if (!SingleLabel)
        ResolverAddCandidate(Config, Name, NULL);

    ResolverReadRegistryString(L"SearchList", SearchList, ARRAYSIZE(SearchList));
    if (SearchList[0])
    {
        PWSTR Context = NULL, Token = wcstok_s(SearchList, L", ", &Context);

        while (Token)
        {
            ResolverAddCandidate(Config, Name, Token);
            Token = wcstok_s(NULL, L", ", &Context);
        }
    }
    else
    {
        ResolverReadRegistryString(L"Domain", Domain, ARRAYSIZE(Domain));
        if (Domain[0])
            ResolverAddCandidate(Config, Name, Domain);
        for (i = 0; i < Config->AdapterCount; i++)
        {
            if (Config->Adapters[i].Suffix[0])
                ResolverAddCandidate(Config, Name, Config->Adapters[i].Suffix);
        }
        for (i = 0; i < Config->AdapterCount; i++)
        {
            PWSTR Context = NULL, Token = wcstok_s(Config->Adapters[i].SearchList, L", ", &Context);

            while (Token)
            {
                ResolverAddCandidate(Config, Name, Token);
                Token = wcstok_s(NULL, L", ", &Context);
            }
        }
    }

    if (!Config->CandidateCount && !SingleLabel)
        ResolverAddCandidate(Config, Name, NULL);
    return ERROR_SUCCESS;
}

static
INT
ResolverSockaddrLength(const SOCKADDR_INET *Address)
{
    return Address->si_family == AF_INET6 ? sizeof(SOCKADDR_IN6) : sizeof(SOCKADDR_IN);
}

static
BOOL
ResolverSameAddress(const SOCKADDR_INET *A, const SOCKADDR_INET *B)
{
    if (A->si_family != B->si_family)
        return FALSE;
    if (A->si_family == AF_INET)
        return A->Ipv4.sin_addr.s_addr == B->Ipv4.sin_addr.s_addr && A->Ipv4.sin_port == B->Ipv4.sin_port;
    return !memcmp(&A->Ipv6.sin6_addr, &B->Ipv6.sin6_addr, sizeof(IN6_ADDR)) && A->Ipv6.sin6_port == B->Ipv6.sin6_port;
}

static
BOOL
ResolverTcpQuery(const SOCKADDR_INET *Server, const UCHAR *Query, INT QueryLength, WORD Id, PCWSTR Name, WORD Type,
                 PRESOLVER_REPLY Reply)
{
    SOCKET Socket = socket(Server->si_family, SOCK_STREAM, IPPROTO_TCP);
    DWORD Timeout = DNS_TCP_TIMEOUT;
    UCHAR *Buffer = NULL;
    UCHAR Prefix[2];
    INT Got = 0, Total = 0, n;
    BOOL Done = FALSE;

    if (Socket == INVALID_SOCKET)
        return FALSE;
    setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (PCHAR)&Timeout, sizeof(Timeout));
    setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (PCHAR)&Timeout, sizeof(Timeout));
    Prefix[0] = (UCHAR)(QueryLength >> 8);
    Prefix[1] = (UCHAR)QueryLength;
    if (connect(Socket, (const SOCKADDR *)Server, ResolverSockaddrLength(Server)) == 0 &&
        send(Socket, (PCHAR)Prefix, 2, 0) == 2 && send(Socket, (PCHAR)Query, QueryLength, 0) == QueryLength)
    {
        while (Got < 2 && (n = recv(Socket, (PCHAR)Prefix + Got, 2 - Got, 0)) > 0)
            Got += n;
        if (Got == 2)
        {
            Total = (Prefix[0] << 8) | Prefix[1];
            Buffer = RtlAllocateHeap(RtlGetProcessHeap(), 0, max(Total, 1));
            Got = 0;
            while (Buffer && Got < Total && (n = recv(Socket, (PCHAR)Buffer + Got, Total - Got, 0)) > 0)
                Got += n;
            if (Buffer && Got == Total)
            {
                ResolverParseReply(Buffer, Total, Id, Name, Type, Reply);
                Done = Reply->Final || Reply->Status != ERROR_SUCCESS;
            }
        }
    }
    if (Buffer)
        RtlFreeHeap(RtlGetProcessHeap(), 0, Buffer);
    closesocket(Socket);
    return Done;
}

static
VOID
ResolverSendRound(PRESOLVER_CONFIG Config, SOCKET Sockets[2], const UCHAR *Query, INT QueryLength, ULONG Round)
{
    ULONG i;

    for (i = 0; i < Config->ServerCount; i++)
    {
        PRESOLVER_SERVER Server = &Config->Servers[i];
        PRESOLVER_ADAPTER Adapter = &Config->Adapters[Server->Adapter];
        SOCKET Socket = Sockets[Server->Address.si_family == AF_INET6];
        BOOL First = (i == Adapter->FirstServer);

        if (Adapter->Negative || Server->Failed || Socket == INVALID_SOCKET)
            continue;
        if (Round == 0 && (Server->Adapter != 0 || !First))
            continue;
        if (Round == 1 && (!First || Server->Sent))
            continue;
        sendto(Socket, (PCHAR)Query, QueryLength, 0, (const SOCKADDR *)&Server->Address,
               ResolverSockaddrLength(&Server->Address));
        Server->Sent = TRUE;
    }
}

static
BOOL
ResolverFailOver(PRESOLVER_CONFIG Config, SOCKET Sockets[2], const UCHAR *Query, INT QueryLength)
{
    BOOL Pending = FALSE;
    ULONG i;

    for (i = 0; i < Config->ServerCount; i++)
    {
        PRESOLVER_SERVER Server = &Config->Servers[i];
        SOCKET Socket = Sockets[Server->Address.si_family == AF_INET6];

        if (Config->Adapters[Server->Adapter].Negative || Server->Failed || Socket == INVALID_SOCKET)
            continue;
        if (Server->Sent)
        {
            Pending = TRUE;
            continue;
        }
        sendto(Socket, (PCHAR)Query, QueryLength, 0, (const SOCKADDR *)&Server->Address,
               ResolverSockaddrLength(&Server->Address));
        Server->Sent = TRUE;
        return TRUE;
    }
    return Pending;
}

static
LONG
ResolverFindServer(PRESOLVER_CONFIG Config, const SOCKADDR_INET *From)
{
    ULONG i;

    for (i = 0; i < Config->ServerCount; i++)
    {
        if (ResolverSameAddress(&Config->Servers[i].Address, From))
            return (LONG)i;
    }
    return -1;
}

static
BOOL
ResolverAllNegative(PRESOLVER_CONFIG Config)
{
    ULONG i;

    for (i = 0; i < Config->AdapterCount; i++)
    {
        if (!Config->Adapters[i].Negative)
            return FALSE;
    }
    return TRUE;
}

static
DNS_STATUS
ResolverQueryName(PRESOLVER_CONFIG Config, PCWSTR Name, WORD Type, DWORD Options, PDNS_RECORDW *Result)
{
    static const DWORD Timeouts[] = {1000, 1000, 2000, 4000};
    UCHAR Query[512], Packet[DNS_UDP_SIZE + 256];
    SOCKET Sockets[2];
    DNS_STATUS Negative = ERROR_SUCCESS, Failure = ERROR_TIMEOUT;
    WORD Id;
    INT QueryLength;
    ULONG Round, i;

    for (i = 0; i < Config->AdapterCount; i++)
        Config->Adapters[i].Negative = FALSE;
    for (i = 0; i < Config->ServerCount; i++)
    {
        Config->Servers[i].Sent = FALSE;
        Config->Servers[i].Failed = FALSE;
    }

    if (!RtlGenRandom(&Id, sizeof(Id)))
        return ERROR_INTERNAL_ERROR;
    QueryLength = ResolverBuildQuery(Name, Type, Id, !(Options & DNS_QUERY_NO_RECURSION), Query, sizeof(Query));
    if (QueryLength < 0)
        return ERROR_INVALID_NAME;

    Sockets[0] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    Sockets[1] = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    for (Round = 0; Round < ARRAYSIZE(Timeouts); Round++)
    {
        DWORD Deadline;

        ResolverSendRound(Config, Sockets, Query, QueryLength, Round);
        Deadline = GetTickCount() + Timeouts[Round];

        for (;;)
        {
            DWORD Now = GetTickCount();
            LONG Remaining = (LONG)(Deadline - Now);
            struct timeval Wait;
            fd_set Read;
            INT Ready;

            if (Remaining <= 0)
                break;
            FD_ZERO(&Read);
            if (Sockets[0] != INVALID_SOCKET)
                FD_SET(Sockets[0], &Read);
            if (Sockets[1] != INVALID_SOCKET)
                FD_SET(Sockets[1], &Read);
            Wait.tv_sec = Remaining / 1000;
            Wait.tv_usec = (Remaining % 1000) * 1000;
            Ready = select(0, &Read, NULL, NULL, &Wait);
            if (Ready <= 0)
                break;

            for (i = 0; i < 2; i++)
            {
                SOCKADDR_INET From;
                INT FromLength = sizeof(From), Received;
                RESOLVER_REPLY Reply;
                LONG Server;

                if (Sockets[i] == INVALID_SOCKET || !FD_ISSET(Sockets[i], &Read))
                    continue;
                Received = recvfrom(Sockets[i], (PCHAR)Packet, sizeof(Packet), 0, (SOCKADDR *)&From, &FromLength);
                if (Received <= 0 || (Server = ResolverFindServer(Config, &From)) < 0)
                    continue;

                ResolverParseReply(Packet, Received, Id, Name, Type, &Reply);
                if ((Reply.Final || Reply.Status == DNS_ERROR_BAD_PACKET) && (Packet[2] & 0x02))
                {
                    DnsIntFreeRecordList((PDNS_RECORD)Reply.Records);
                    Reply.Records = NULL;
                    Reply.Final = FALSE;
                    if (!ResolverTcpQuery(&Config->Servers[Server].Address, Query, QueryLength, Id, Name, Type, &Reply))
                        continue;
                }

                if (Reply.Final && Reply.Status == ERROR_SUCCESS)
                {
                    *Result = Reply.Records;
                    closesocket(Sockets[0]);
                    closesocket(Sockets[1]);
                    return ERROR_SUCCESS;
                }
                if (Reply.Final)
                {
                    if (Negative == ERROR_SUCCESS)
                        Negative = Reply.Status;
                    Config->Adapters[Config->Servers[Server].Adapter].Negative = TRUE;
                    if (ResolverAllNegative(Config))
                    {
                        closesocket(Sockets[0]);
                        closesocket(Sockets[1]);
                        return Negative;
                    }
                }
                else if (Reply.Status != ERROR_SUCCESS)
                {
                    Failure = Reply.Status;
                    Config->Servers[Server].Failed = TRUE;
                    if (!ResolverFailOver(Config, Sockets, Query, QueryLength))
                    {
                        closesocket(Sockets[0]);
                        closesocket(Sockets[1]);
                        return Negative != ERROR_SUCCESS ? Negative : Failure;
                    }
                }
            }
        }
    }

    closesocket(Sockets[0]);
    closesocket(Sockets[1]);
    return Negative != ERROR_SUCCESS ? Negative : Failure;
}

static
ULONG
ResolverLlmnrLinks(PULONG Links4, PULONG Count4, PULONG Links6, PULONG Count6)
{
    ULONG Size = 16384;
    PIP_ADAPTER_ADDRESSES List, Adapter;
    DWORD Error;

    *Count4 = *Count6 = 0;
    for (;;)
    {
        List = RtlAllocateHeap(RtlGetProcessHeap(), 0, Size);
        if (!List)
            return 0;
        Error = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                                     GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, List, &Size);
        if (Error != ERROR_BUFFER_OVERFLOW)
            break;
        RtlFreeHeap(RtlGetProcessHeap(), 0, List);
    }
    if (Error != ERROR_SUCCESS)
    {
        RtlFreeHeap(RtlGetProcessHeap(), 0, List);
        return 0;
    }

    for (Adapter = List; Adapter; Adapter = Adapter->Next)
    {
        PIP_ADAPTER_UNICAST_ADDRESS Unicast;
        BOOL Have4 = FALSE, Have6 = FALSE;

        if (Adapter->OperStatus != IfOperStatusUp || Adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
            continue;
        for (Unicast = Adapter->FirstUnicastAddress; Unicast; Unicast = Unicast->Next)
        {
            const SOCKADDR *Address = Unicast->Address.lpSockaddr;

            if (!Address)
                continue;
            if (Address->sa_family == AF_INET && !Have4 && *Count4 < DNS_MAX_ADAPTERS)
            {
                Links4[(*Count4)++] = ((const SOCKADDR_IN *)Address)->sin_addr.s_addr;
                Have4 = TRUE;
            }
            else if (Address->sa_family == AF_INET6 && !Have6 && Adapter->Ipv6IfIndex && *Count6 < DNS_MAX_ADAPTERS)
            {
                Links6[(*Count6)++] = Adapter->Ipv6IfIndex;
                Have6 = TRUE;
            }
        }
    }
    RtlFreeHeap(RtlGetProcessHeap(), 0, List);
    return *Count4 + *Count6;
}

static
DNS_STATUS
ResolverLlmnrQuery(PCWSTR Name, WORD Type, PDNS_RECORDW *Result)
{
    ULONG Links4[DNS_MAX_ADAPTERS], Links6[DNS_MAX_ADAPTERS], Count4, Count6, i;
    UCHAR Query[512], Packet[DNS_UDP_SIZE + 256];
    SOCKADDR_IN Group4;
    SOCKADDR_IN6 Group6;
    SOCKET Sockets[2];
    DWORD Start, Hops = 1;
    INT QueryLength, Round;
    WORD Id;

    if (!ResolverLlmnrLinks(Links4, &Count4, Links6, &Count6))
        return ERROR_INVALID_NAME;

    if (!RtlGenRandom(&Id, sizeof(Id)))
        return ERROR_INTERNAL_ERROR;
    QueryLength = ResolverBuildQuery(Name, Type, Id, FALSE, Query, sizeof(Query));
    if (QueryLength < 0)
        return ERROR_INVALID_NAME;

    RtlZeroMemory(&Group4, sizeof(Group4));
    Group4.sin_family = AF_INET;
    Group4.sin_port = htons(LLMNR_PORT);
    Group4.sin_addr.s_addr = htonl(0xE00000FC);
    RtlZeroMemory(&Group6, sizeof(Group6));
    Group6.sin6_family = AF_INET6;
    Group6.sin6_port = htons(LLMNR_PORT);
    Group6.sin6_addr.s6_addr[0] = 0xFF;
    Group6.sin6_addr.s6_addr[1] = 0x02;
    Group6.sin6_addr.s6_addr[13] = 0x01;
    Group6.sin6_addr.s6_addr[15] = 0x03;

    Sockets[0] = Count4 ? socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) : INVALID_SOCKET;
    Sockets[1] = Count6 ? socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP) : INVALID_SOCKET;
    if (Sockets[0] != INVALID_SOCKET)
        setsockopt(Sockets[0], IPPROTO_IP, IP_MULTICAST_TTL, (PCHAR)&Hops, sizeof(Hops));
    if (Sockets[1] != INVALID_SOCKET)
        setsockopt(Sockets[1], IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (PCHAR)&Hops, sizeof(Hops));

    Start = GetTickCount();
    for (Round = 0; Round < LLMNR_ROUNDS; Round++)
    {
        DWORD Deadline = Start + (Round + 1) * LLMNR_RESEND;

        for (i = 0; i < Count4 && Sockets[0] != INVALID_SOCKET; i++)
        {
            setsockopt(Sockets[0], IPPROTO_IP, IP_MULTICAST_IF, (PCHAR)&Links4[i], sizeof(Links4[i]));
            sendto(Sockets[0], (PCHAR)Query, QueryLength, 0, (const SOCKADDR *)&Group4, sizeof(Group4));
        }
        for (i = 0; i < Count6 && Sockets[1] != INVALID_SOCKET; i++)
        {
            setsockopt(Sockets[1], IPPROTO_IPV6, IPV6_MULTICAST_IF, (PCHAR)&Links6[i], sizeof(Links6[i]));
            Group6.sin6_scope_id = Links6[i];
            sendto(Sockets[1], (PCHAR)Query, QueryLength, 0, (const SOCKADDR *)&Group6, sizeof(Group6));
        }

        for (;;)
        {
            LONG Remaining = (LONG)(Deadline - GetTickCount());
            struct timeval Wait;
            fd_set Read;
            ULONG s;

            if (Remaining <= 0)
                break;
            FD_ZERO(&Read);
            for (s = 0; s < 2; s++)
            {
                if (Sockets[s] != INVALID_SOCKET)
                    FD_SET(Sockets[s], &Read);
            }
            Wait.tv_sec = Remaining / 1000;
            Wait.tv_usec = (Remaining % 1000) * 1000;
            if (select(0, &Read, NULL, NULL, &Wait) <= 0)
                break;

            for (s = 0; s < 2; s++)
            {
                RESOLVER_REPLY Reply;
                INT Received;

                if (Sockets[s] == INVALID_SOCKET || !FD_ISSET(Sockets[s], &Read))
                    continue;
                Received = recvfrom(Sockets[s], (PCHAR)Packet, sizeof(Packet), 0, NULL, NULL);
                if (Received <= 0)
                    continue;
                ResolverParseReply(Packet, Received, Id, Name, Type, &Reply);
                if (Reply.Final && Reply.Status == ERROR_SUCCESS && Reply.Records)
                {
                    *Result = Reply.Records;
                    if (Sockets[0] != INVALID_SOCKET)
                        closesocket(Sockets[0]);
                    if (Sockets[1] != INVALID_SOCKET)
                        closesocket(Sockets[1]);
                    return ERROR_SUCCESS;
                }
                DnsIntFreeRecordList((PDNS_RECORD)Reply.Records);
            }
        }
    }

    if (Sockets[0] != INVALID_SOCKET)
        closesocket(Sockets[0]);
    if (Sockets[1] != INVALID_SOCKET)
        closesocket(Sockets[1]);
    return ERROR_INVALID_NAME;
}

DNS_STATUS
DnsIntResolve(LPCWSTR Name, WORD Type, DWORD Options, PDNS_RECORDW *Result)
{
    PRESOLVER_CONFIG Config;
    DNS_STATUS Status, First = ERROR_SUCCESS;
    ULONG i;

    *Result = NULL;
    if (Options & DNS_QUERY_NO_WIRE_QUERY)
        return DNS_ERROR_RCODE_NAME_ERROR;

    ResolverStartWinsock();
    if (Options & DNS_QUERY_MULTICAST_ONLY)
        return wcschr(Name, L'.') ? ERROR_INVALID_NAME : ResolverLlmnrQuery(Name, Type, Result);
    Config = RtlAllocateHeap(RtlGetProcessHeap(), 0, sizeof(*Config));
    if (!Config)
        return ERROR_OUTOFMEMORY;

    Status = ResolverLoadConfig(Config, Name);
    if (Status != ERROR_SUCCESS && (Status != DNS_ERROR_NO_DNS_SERVERS || wcschr(Name, L'.')))
    {
        RtlFreeHeap(RtlGetProcessHeap(), 0, Config);
        return Status;
    }
    if (Status != ERROR_SUCCESS)
        Config->CandidateCount = 0;

    for (i = 0; i < Config->CandidateCount; i++)
    {
        Status = ResolverQueryName(Config, Config->Candidates[i], Type, Options, Result);
        DPRINT("DnsIntResolve(%S, %u) -> %lu\n", Config->Candidates[i], Type, Status);
        if (Status == ERROR_SUCCESS)
            break;
        if (First == ERROR_SUCCESS)
            First = Status;
        if (Status != DNS_ERROR_RCODE_NAME_ERROR && Status != DNS_INFO_NO_RECORDS)
            break;
    }

    if (!wcschr(Name, L'.') && (!Config->CandidateCount || Status != ERROR_SUCCESS))
    {
        Status = (Options & DNS_QUERY_NO_MULTICAST) ? ERROR_INVALID_NAME : ResolverLlmnrQuery(Name, Type, Result);
        if (Status != ERROR_SUCCESS && First == ERROR_SUCCESS)
            First = Status;
    }

    RtlFreeHeap(RtlGetProcessHeap(), 0, Config);
    return Status == ERROR_SUCCESS ? Status : First;
}
