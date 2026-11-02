/*
 * PROJECT:     LiberNT DNS API
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     DNS message encoding and response parsing
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif193@gmail.com>
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <stdarg.h>

#define WIN32_NO_STATUS
#define _INC_WINDOWS
#define COM_NO_WINDOWS_H
#include <windef.h>
#include <winbase.h>
#include <winnls.h>
#include <windns.h>
#define NTOS_MODE_USER
#include <ndk/rtlfuncs.h>
#include <strsafe.h>

#include "message.h"

INT
ResolverBuildQuery(PCWSTR Name, WORD Type, WORD Id, BOOL Recursion, PUCHAR Packet, INT Capacity)
{
    CHAR Utf8[DNS_MAX_NAME_BUFFER_LENGTH * 3];
    PCHAR Label, Next;
    INT Offset = 12;

    if (!WideCharToMultiByte(CP_UTF8, 0, Name, -1, Utf8, sizeof(Utf8), NULL, NULL))
        return -1;

    Packet[0] = (UCHAR)(Id >> 8);
    Packet[1] = (UCHAR)Id;
    Packet[2] = Recursion ? 0x01 : 0x00;
    Packet[3] = 0;
    Packet[4] = 0; Packet[5] = 1;
    Packet[6] = Packet[7] = Packet[8] = Packet[9] = Packet[10] = Packet[11] = 0;

    for (Label = Utf8; *Label; Label = Next)
    {
        SIZE_T Length;

        Next = strchr(Label, '.');
        Length = Next ? (SIZE_T)(Next - Label) : strlen(Label);
        if (!Length || Length > 63 || Offset + (INT)Length + 1 >= Capacity - 5)
            return -1;
        Packet[Offset++] = (UCHAR)Length;
        RtlCopyMemory(Packet + Offset, Label, Length);
        Offset += (INT)Length;
        if (!Next)
            break;
        Next++;
    }
    Packet[Offset++] = 0;
    Packet[Offset++] = (UCHAR)(Type >> 8);
    Packet[Offset++] = (UCHAR)Type;
    Packet[Offset++] = 0;
    Packet[Offset++] = 1;
    return Offset;
}

INT
ResolverReadName(const UCHAR *Packet, INT Length, INT Offset, PWSTR Name, INT Count)
{
    CHAR Utf8[DNS_MAX_NAME_BUFFER_LENGTH * 3];
    INT Out = 0, End = -1, Jumps = 0, Wire = 1;

    for (;;)
    {
        UCHAR Label;

        if (Offset >= Length)
            return -1;
        Label = Packet[Offset];
        if ((Label & 0xC0) == 0xC0)
        {
            if (Offset + 1 >= Length || ++Jumps > 32)
                return -1;
            if (End < 0)
                End = Offset + 2;
            Offset = ((Label & 0x3F) << 8) | Packet[Offset + 1];
            continue;
        }
        if (Label & 0xC0)
            return -1;
        Offset++;
        if (!Label)
            break;
        Wire += Label + 1;
        if (Wire > DNS_MAX_NAME_LENGTH || Offset + Label > Length || Out + Label + 2 >= (INT)sizeof(Utf8))
            return -1;
        if (Out)
            Utf8[Out++] = '.';
        RtlCopyMemory(Utf8 + Out, Packet + Offset, Label);
        Out += Label;
        Offset += Label;
    }
    Utf8[Out] = 0;
    if (!MultiByteToWideChar(CP_UTF8, 0, Utf8, -1, Name, Count))
        return -1;
    return End >= 0 ? End : Offset;
}

static
PWSTR
ResolverDupName(PCWSTR Name)
{
    SIZE_T Size = (wcslen(Name) + 1) * sizeof(WCHAR);
    PWSTR Copy = RtlAllocateHeap(RtlGetProcessHeap(), 0, Size);

    if (Copy)
        RtlCopyMemory(Copy, Name, Size);
    return Copy;
}

static
DNS_STATUS
ResolverReadDataName(const UCHAR *Packet, INT Length, INT Offset, INT End, PWSTR *Result, PINT Next)
{
    WCHAR Name[DNS_MAX_NAME_BUFFER_LENGTH];
    INT After = ResolverReadName(Packet, Length, Offset, Name, ARRAYSIZE(Name));

    if (After < 0 || After > End)
        return DNS_ERROR_BAD_PACKET;
    *Result = ResolverDupName(Name);
    if (!*Result)
        return ERROR_OUTOFMEMORY;
    if (Next)
        *Next = After;
    return ERROR_SUCCESS;
}

static
WORD
ResolverWord(const UCHAR *P)
{
    return (WORD)((P[0] << 8) | P[1]);
}

static
DWORD
ResolverDword(const UCHAR *P)
{
    return ((DWORD)P[0] << 24) | ((DWORD)P[1] << 16) | ((DWORD)P[2] << 8) | P[3];
}

static
DNS_STATUS
ResolverParseRecord(const UCHAR *Packet, INT Length, PCWSTR Owner, WORD Type, DWORD Ttl, INT Data, WORD DataLength,
                    PDNS_RECORDW *Result)
{
    PDNS_RECORDW Record;
    SIZE_T Size = sizeof(DNS_RECORDW);
    INT End = Data + DataLength;
    DWORD Strings = 0;
    DNS_STATUS Status;
    BOOL Valid;

    *Result = NULL;

    switch (Type)
    {
    case DNS_TYPE_A:
        Valid = DataLength == sizeof(IP4_ADDRESS);
        break;

    case DNS_TYPE_AAAA:
        Valid = DataLength == sizeof(IP6_ADDRESS);
        break;

    case DNS_TYPE_CNAME:
    case DNS_TYPE_PTR:
    case DNS_TYPE_NS:
    case DNS_TYPE_SOA:
        Valid = TRUE;
        break;

    case DNS_TYPE_MX:
        Valid = DataLength > 2;
        break;

    case DNS_TYPE_SRV:
        Valid = DataLength > 6;
        break;

    case DNS_TYPE_TEXT:
    {
        INT Offset = Data;

        while (Offset < End)
        {
            Offset += Packet[Offset] + 1;
            Strings++;
        }
        Valid = Offset == End && Strings && FIELD_OFFSET(DNS_TXT_DATAW, pStringArray[Strings]) <= MAXWORD;
        Size = FIELD_OFFSET(DNS_RECORDW, Data.TXT.pStringArray[Strings]);
        break;
    }

    default:
        return ERROR_SUCCESS;
    }

    if (!Valid)
        return DNS_ERROR_BAD_PACKET;

    Record = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY, max(Size, sizeof(DNS_RECORDW)));
    if (!Record)
        return ERROR_OUTOFMEMORY;
    Record->pName = ResolverDupName(Owner);
    Record->wType = Type;
    Record->Flags.S.Section = DnsSectionAnswer;
    Record->Flags.S.CharSet = DnsCharSetUnicode;
    Record->dwTtl = Ttl;
    Status = Record->pName ? ERROR_SUCCESS : ERROR_OUTOFMEMORY;

    switch (Type)
    {
    case DNS_TYPE_A:
        RtlCopyMemory(&Record->Data.A.IpAddress, Packet + Data, sizeof(IP4_ADDRESS));
        Record->wDataLength = sizeof(DNS_A_DATA);
        break;

    case DNS_TYPE_AAAA:
        RtlCopyMemory(&Record->Data.AAAA.Ip6Address, Packet + Data, sizeof(IP6_ADDRESS));
        Record->wDataLength = sizeof(DNS_AAAA_DATA);
        break;

    case DNS_TYPE_CNAME:
    case DNS_TYPE_PTR:
    case DNS_TYPE_NS:
        if (Status == ERROR_SUCCESS)
            Status = ResolverReadDataName(Packet, Length, Data, End, &Record->Data.PTR.pNameHost, NULL);
        Record->wDataLength = sizeof(DNS_PTR_DATAW);
        break;

    case DNS_TYPE_MX:
        if (Status == ERROR_SUCCESS)
            Status = ResolverReadDataName(Packet, Length, Data + 2, End, &Record->Data.MX.pNameExchange, NULL);
        Record->Data.MX.wPreference = ResolverWord(Packet + Data);
        Record->wDataLength = sizeof(DNS_MX_DATAW);
        break;

    case DNS_TYPE_SRV:
        if (Status == ERROR_SUCCESS)
            Status = ResolverReadDataName(Packet, Length, Data + 6, End, &Record->Data.SRV.pNameTarget, NULL);
        Record->Data.SRV.wPriority = ResolverWord(Packet + Data);
        Record->Data.SRV.wWeight = ResolverWord(Packet + Data + 2);
        Record->Data.SRV.wPort = ResolverWord(Packet + Data + 4);
        Record->wDataLength = sizeof(DNS_SRV_DATAW);
        break;

    case DNS_TYPE_SOA:
    {
        INT Offset = Data;

        if (Status == ERROR_SUCCESS)
            Status = ResolverReadDataName(Packet, Length, Offset, End, &Record->Data.SOA.pNamePrimaryServer, &Offset);
        if (Status == ERROR_SUCCESS)
            Status = ResolverReadDataName(Packet, Length, Offset, End, &Record->Data.SOA.pNameAdministrator, &Offset);
        if (Status == ERROR_SUCCESS && Offset + 20 > End)
            Status = DNS_ERROR_BAD_PACKET;
        if (Status == ERROR_SUCCESS)
        {
            Record->Data.SOA.dwSerialNo = ResolverDword(Packet + Offset);
            Record->Data.SOA.dwRefresh = ResolverDword(Packet + Offset + 4);
            Record->Data.SOA.dwRetry = ResolverDword(Packet + Offset + 8);
            Record->Data.SOA.dwExpire = ResolverDword(Packet + Offset + 12);
            Record->Data.SOA.dwDefaultTtl = ResolverDword(Packet + Offset + 16);
        }
        Record->wDataLength = sizeof(DNS_SOA_DATAW);
        break;
    }

    case DNS_TYPE_TEXT:
    {
        INT Offset = Data;
        DWORD i;

        Record->Data.TXT.dwStringCount = Strings;
        for (i = 0; Status == ERROR_SUCCESS && i < Strings; i++)
        {
            UCHAR StringLength = Packet[Offset++];
            INT Chars = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Packet + Offset, StringLength, NULL, 0);
            PWSTR String = RtlAllocateHeap(RtlGetProcessHeap(), 0, (Chars + 1) * sizeof(WCHAR));

            if (!String)
            {
                Status = ERROR_OUTOFMEMORY;
                break;
            }
            MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Packet + Offset, StringLength, String, Chars);
            String[Chars] = UNICODE_NULL;
            Record->Data.TXT.pStringArray[i] = String;
            Offset += StringLength;
        }
        Record->wDataLength = (WORD)(FIELD_OFFSET(DNS_TXT_DATAW, pStringArray[Strings]));
        break;
    }
    }

    if (Status != ERROR_SUCCESS)
    {
        DnsIntFreeRecordList((PDNS_RECORD)Record);
        return Status;
    }
    *Result = Record;
    return ERROR_SUCCESS;
}

VOID
ResolverParseReply(const UCHAR *Packet, INT Length, WORD Id, PCWSTR Name, WORD Type, PRESOLVER_REPLY Reply)
{
    WCHAR Owner[DNS_MAX_NAME_BUFFER_LENGTH], Target[DNS_MAX_NAME_BUFFER_LENGTH];
    PDNS_RECORDW *Tail = &Reply->Records;
    WORD Questions, Answers, i;
    INT Offset;
    BOOL Matched = FALSE;

    Reply->Status = ERROR_SUCCESS;
    Reply->Final = FALSE;
    Reply->Records = NULL;

    if (Length < 12 || ResolverWord(Packet) != Id || !(Packet[2] & 0x80))
        return;

    Questions = ResolverWord(Packet + 4);
    Answers = ResolverWord(Packet + 6);
    Offset = 12;
    if (Questions > 1 || (Questions == 0 && !(Packet[3] & 0x0F)))
        return;
    if (Questions == 1)
    {
        Offset = ResolverReadName(Packet, Length, Offset, Owner, ARRAYSIZE(Owner));
        if (Offset < 0 || Offset + 4 > Length || _wcsicmp(Owner, Name) || ResolverWord(Packet + Offset) != Type)
            return;
        Offset += 4;
    }

    switch (Packet[3] & 0x0F)
    {
    case 0:
        break;
    case 3:
        Reply->Status = DNS_ERROR_RCODE_NAME_ERROR;
        Reply->Final = TRUE;
        return;
    case 1:
        Reply->Status = DNS_ERROR_RCODE_FORMAT_ERROR;
        return;
    case 5:
        Reply->Status = DNS_ERROR_RCODE_REFUSED;
        return;
    default:
        Reply->Status = DNS_ERROR_RCODE_SERVER_FAILURE;
        return;
    }

    StringCchCopyW(Target, ARRAYSIZE(Target), Name);
    for (i = 0; i < Answers; i++)
    {
        WORD RecordType, Class, DataLength;
        DWORD Ttl;
        PDNS_RECORDW Record;

        Offset = ResolverReadName(Packet, Length, Offset, Owner, ARRAYSIZE(Owner));
        if (Offset < 0 || Offset + 10 > Length)
        {
            Reply->Status = DNS_ERROR_BAD_PACKET;
            break;
        }
        RecordType = ResolverWord(Packet + Offset);
        Class = ResolverWord(Packet + Offset + 2);
        Ttl = ResolverDword(Packet + Offset + 4);
        DataLength = ResolverWord(Packet + Offset + 8);
        Offset += 10;
        if (Offset + DataLength > Length)
        {
            Reply->Status = DNS_ERROR_BAD_PACKET;
            break;
        }

        if (Class == 1 && (RecordType == Type || RecordType == DNS_TYPE_CNAME))
        {
            Reply->Status = ResolverParseRecord(Packet, Length, Owner, RecordType, Ttl, Offset, DataLength, &Record);
            if (Reply->Status != ERROR_SUCCESS)
                break;
            if (Record)
            {
                *Tail = Record;
                Tail = &Record->pNext;
                if (RecordType == DNS_TYPE_CNAME && !_wcsicmp(Owner, Target))
                    StringCchCopyW(Target, ARRAYSIZE(Target), Record->Data.CNAME.pNameHost);
                if (RecordType == Type)
                    Matched = TRUE;
            }
        }
        Offset += DataLength;
    }

    if (Reply->Status != ERROR_SUCCESS)
    {
        DnsIntFreeRecordList((PDNS_RECORD)Reply->Records);
        Reply->Records = NULL;
        return;
    }

    Reply->Final = TRUE;
    if (!Matched)
    {
        DnsIntFreeRecordList((PDNS_RECORD)Reply->Records);
        Reply->Records = NULL;
        Reply->Status = DNS_INFO_NO_RECORDS;
    }
}
