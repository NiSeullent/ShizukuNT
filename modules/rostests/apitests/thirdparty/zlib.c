/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the bundled zlib library
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>
#include <string.h>
#include <zlib.h>

typedef struct _CRC_COMBINE_RESULT
{
    uLong Combined;
    uLong Operator;
} CRC_COMBINE_RESULT;

static DWORD WINAPI
CrcCombineNegativeThread(LPVOID Parameter)
{
    CRC_COMBINE_RESULT *Result = Parameter;

    Result->Combined = crc32_combine(0x12345678UL, 0x9abcdef0UL, -1);
    Result->Operator = crc32_combine_gen(-1);
    return 0;
}

static void
Test_Version(void)
{
    ok(strcmp(zlibVersion(), ZLIB_VERSION) == 0,
       "zlibVersion() returned '%s', header is '%s'\n", zlibVersion(), ZLIB_VERSION);
}

static void
Test_Crc32Combine(void)
{
    static const char First[] = "hello ";
    static const char Second[] = "world";
    static const char Both[] = "hello world";
    uLong Crc1, Crc2, CrcBoth;

    Crc1 = crc32(0, (const Bytef *)First, sizeof(First) - 1);
    Crc2 = crc32(0, (const Bytef *)Second, sizeof(Second) - 1);
    CrcBoth = crc32(0, (const Bytef *)Both, sizeof(Both) - 1);

    ok(CrcBoth == 0x0d4a1185UL, "crc32 of '%s' is 0x%08lx\n", Both, CrcBoth);
    ok(crc32_combine(Crc1, Crc2, sizeof(Second) - 1) == CrcBoth,
       "crc32_combine returned 0x%08lx, expected 0x%08lx\n",
       crc32_combine(Crc1, Crc2, sizeof(Second) - 1), CrcBoth);
    ok(crc32_combine_op(Crc1, Crc2, crc32_combine_gen(sizeof(Second) - 1)) == CrcBoth,
       "crc32_combine_op returned a different value than crc32_combine\n");
}

static void
Test_Crc32CombineNegativeLength(void)
{
    CRC_COMBINE_RESULT Result = { 1, 1 };
    HANDLE Thread;
    DWORD Wait;

    Thread = CreateThread(NULL, 0, CrcCombineNegativeThread, &Result, 0, NULL);
    ok(Thread != NULL, "CreateThread failed with %lu\n", GetLastError());
    if (!Thread)
        return;

    Wait = WaitForSingleObject(Thread, 10000);
    ok(Wait == WAIT_OBJECT_0, "crc32_combine with a negative length did not return (wait %lu)\n", Wait);
    if (Wait == WAIT_OBJECT_0)
    {
        ok(Result.Combined == 0, "crc32_combine(-1) returned 0x%08lx\n", Result.Combined);
        ok(Result.Operator == 0, "crc32_combine_gen(-1) returned 0x%08lx\n", Result.Operator);
    }
    else
    {
        TerminateThread(Thread, 0);
    }
    CloseHandle(Thread);
}

static void
Test_CompressRoundTrip(void)
{
    static const char Text[] = "LiberNT zlib round trip LiberNT zlib round trip LiberNT zlib round trip";
    Bytef Compressed[256];
    Bytef Output[sizeof(Text) + 16];
    uLongf CompressedLength = sizeof(Compressed);
    uLongf OutputLength;
    int Status;

    ok(compressBound(sizeof(Text)) <= sizeof(Compressed),
       "compressBound returned %lu\n", compressBound(sizeof(Text)));

    Status = compress(Compressed, &CompressedLength, (const Bytef *)Text, sizeof(Text));
    ok(Status == Z_OK, "compress returned %d\n", Status);
    if (Status != Z_OK)
        return;

    OutputLength = sizeof(Output);
    memset(Output, 0xcc, sizeof(Output));
    Status = uncompress(Output, &OutputLength, Compressed, CompressedLength);
    ok(Status == Z_OK, "uncompress returned %d\n", Status);
    ok(OutputLength == sizeof(Text), "uncompress produced %lu bytes\n", OutputLength);
    ok(memcmp(Output, Text, sizeof(Text)) == 0, "uncompress produced different data\n");
    ok(Output[sizeof(Text)] == 0xcc, "uncompress wrote past the decompressed data\n");

    OutputLength = sizeof(Output);
    Status = uncompress(Output, &OutputLength, Compressed, CompressedLength - 1);
    ok(Status == Z_DATA_ERROR, "uncompress of a truncated stream returned %d\n", Status);

    OutputLength = sizeof(Text) - 1;
    Status = uncompress(Output, &OutputLength, Compressed, CompressedLength);
    ok(Status == Z_BUF_ERROR, "uncompress into a short buffer returned %d\n", Status);
}

START_TEST(zlib)
{
    Test_Version();
    Test_Crc32Combine();
    Test_Crc32CombineNegativeLength();
    Test_CompressRoundTrip();
}
