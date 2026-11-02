/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the bundled libmpg123 library
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>
#include <string.h>
#include <mpg123.h>

#define MP3_FRAME_BYTES 417
#define MP3_FRAME_COUNT 4
#define MP3_SAMPLES_PER_FRAME 1152

typedef struct _NTOM_RESULT
{
    int ReadResult;
    int ErrorCode;
} NTOM_RESULT;

static unsigned char SilentStream[MP3_FRAME_BYTES * MP3_FRAME_COUNT];

static void
BuildSilentStream(void)
{
    /* MPEG-1 Layer III, no CRC, 128 kbit/s, 44100 Hz, mono, zeroed side info and main data. */
    static const unsigned char Header[4] = { 0xFF, 0xFB, 0x90, 0xC0 };
    unsigned int Frame;

    memset(SilentStream, 0, sizeof(SilentStream));
    for (Frame = 0; Frame < MP3_FRAME_COUNT; Frame++)
        memcpy(SilentStream + Frame * MP3_FRAME_BYTES, Header, sizeof(Header));
}

static mpg123_handle *
OpenQuietFeed(void)
{
    mpg123_handle *Handle;
    int Error = MPG123_OK;

    Handle = mpg123_new(NULL, &Error);
    ok(Handle != NULL, "mpg123_new failed with %d\n", Error);
    if (!Handle)
        return NULL;

    ok(mpg123_param2(Handle, MPG123_ADD_FLAGS, MPG123_QUIET, 0.0) == MPG123_OK,
       "MPG123_QUIET was not accepted\n");
    ok(mpg123_open_feed(Handle) == MPG123_OK, "mpg123_open_feed failed\n");
    return Handle;
}

static void
Test_Version(void)
{
    unsigned int PatchLevel = 0, Major = 0, Minor = 0, Patch = 0;
    unsigned int Api;
    const char *Version;

    Api = mpg123_libversion(&PatchLevel);
    ok(Api == MPG123_API_VERSION, "mpg123_libversion returned %u, header is %u\n",
       Api, (unsigned int)MPG123_API_VERSION);
    ok(PatchLevel == MPG123_PATCHLEVEL, "patch level is %u, header is %u\n",
       PatchLevel, (unsigned int)MPG123_PATCHLEVEL);

    Version = mpg123_distversion(&Major, &Minor, &Patch);
    ok(Version != NULL, "mpg123_distversion returned NULL\n");
    ok(Major > 1 || (Major == 1 && (Minor > 33 || (Minor == 33 && Patch >= 7))),
       "libmpg123 %u.%u.%u predates the 1.33.7 security fixes\n", Major, Minor, Patch);
}

static void
Test_Features(void)
{
    ok(mpg123_feature2(MPG123_FEATURE_ABI_UTF8OPEN) == 1, "UTF-8 file names are not expected\n");
    ok(mpg123_feature2(MPG123_FEATURE_DECODE_LAYER1) == 1, "Layer I decoding is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_DECODE_LAYER2) == 1, "Layer II decoding is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_DECODE_LAYER3) == 1, "Layer III decoding is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_DECODE_NTOM) == 1, "NtoM resampling is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_OUTPUT_16BIT) == 1, "16-bit output is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_OUTPUT_32BIT) == 1, "32-bit output is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_OUTPUT_FLOAT32) == 1, "float output is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_PARSE_ID3V2) == 1, "ID3v2 parsing is missing\n");
    ok(mpg123_feature2(MPG123_FEATURE_INDEX) == 1, "the frame index is missing\n");
}

static void
Test_Decoders(void)
{
    const char **Decoders;
    mpg123_handle *Handle;
    const char *Current;

    Decoders = mpg123_decoders();
    ok(Decoders != NULL && Decoders[0] != NULL, "mpg123_decoders returned no decoder\n");
    if (Decoders && Decoders[0])
    {
        ok(strcmp(Decoders[0], "generic") == 0, "first decoder is '%s'\n", Decoders[0]);
        ok(Decoders[1] == NULL, "unexpected second decoder '%s'\n", Decoders[1]);
    }

    Handle = OpenQuietFeed();
    if (!Handle)
        return;

    Current = mpg123_current_decoder(Handle);
    ok(Current != NULL && strcmp(Current, "generic") == 0,
       "current decoder is '%s'\n", Current ? Current : "(null)");

    ok(mpg123_decoder(Handle, "no-such-decoder") == MPG123_ERR,
       "an unknown decoder name was accepted\n");
    ok(mpg123_errcode(Handle) == MPG123_BAD_DECODER, "error code is %d\n", mpg123_errcode(Handle));

    ok(mpg123_decoder(Handle, "SSE") == MPG123_OK, "the single built decoder was not kept\n");
    Current = mpg123_current_decoder(Handle);
    ok(Current != NULL && strcmp(Current, "generic") == 0,
       "current decoder is '%s' after asking for SSE\n", Current ? Current : "(null)");

    mpg123_delete(Handle);
}

static int
IsSilentSample(int OutEncoding, const unsigned char *Sample)
{
    float FloatSample;
    short ShortSample;

    if (OutEncoding == MPG123_ENC_FLOAT_32)
    {
        memcpy(&FloatSample, Sample, sizeof(FloatSample));
        return FloatSample == 0.0f;
    }
    memcpy(&ShortSample, Sample, sizeof(ShortSample));
    return ShortSample == 0;
}

static void
DecodeSilence(int OutEncoding, size_t SampleSize)
{
    static unsigned char Pcm[sizeof(float) * MP3_SAMPLES_PER_FRAME * MP3_FRAME_COUNT];
    const size_t Capacity = SampleSize * MP3_SAMPLES_PER_FRAME * MP3_FRAME_COUNT;
    struct mpg123_frameinfo2 Info;
    mpg123_handle *Handle;
    size_t Done, Total = 0, Index, NonZero = 0;
    long Rate = 0;
    int Channels = 0, Encoding = 0, Ret = MPG123_OK, Loop;

    Handle = OpenQuietFeed();
    if (!Handle)
        return;

    ok(mpg123_param2(Handle, MPG123_REMOVE_FLAGS, MPG123_GAPLESS, 0.0) == MPG123_OK,
       "MPG123_GAPLESS could not be removed\n");
    ok(mpg123_format_none(Handle) == MPG123_OK, "mpg123_format_none failed\n");
    ok(mpg123_format(Handle, 44100, MPG123_MONO, OutEncoding) == MPG123_OK,
       "mpg123_format failed for encoding 0x%x\n", OutEncoding);
    ok(mpg123_feed(Handle, SilentStream, sizeof(SilentStream)) == MPG123_OK, "mpg123_feed failed\n");

    for (Loop = 0; Loop < 16 && Total < Capacity; Loop++)
    {
        Done = 0;
        Ret = mpg123_read(Handle, Pcm + Total, Capacity - Total, &Done);
        Total += Done;
        if (Ret != MPG123_OK && Ret != MPG123_NEW_FORMAT)
            break;
    }

    ok(Ret == MPG123_NEED_MORE || Ret == MPG123_OK, "mpg123_read returned %d (%s)\n",
       Ret, mpg123_strerror(Handle));
    ok(Total >= MP3_SAMPLES_PER_FRAME * SampleSize * (MP3_FRAME_COUNT - 1),
       "decoded %lu bytes from %u frames\n", (unsigned long)Total, MP3_FRAME_COUNT);
    ok(Total % (MP3_SAMPLES_PER_FRAME * SampleSize) == 0,
       "decoded %lu bytes, not a whole number of frames\n", (unsigned long)Total);

    for (Index = 0; Index < Total / SampleSize; Index++)
    {
        if (!IsSilentSample(OutEncoding, Pcm + Index * SampleSize))
            NonZero++;
    }
    ok(NonZero == 0, "encoding 0x%x: %lu of %lu decoded samples are not silent\n",
       OutEncoding, (unsigned long)NonZero, (unsigned long)(Total / SampleSize));

    ok(mpg123_getformat(Handle, &Rate, &Channels, &Encoding) == MPG123_OK, "mpg123_getformat failed\n");
    ok(Rate == 44100, "rate is %ld\n", Rate);
    ok(Channels == MPG123_MONO, "channel count is %d\n", Channels);
    ok(Encoding == OutEncoding, "encoding is 0x%x, expected 0x%x\n", Encoding, OutEncoding);
    ok(mpg123_spf(Handle) == MP3_SAMPLES_PER_FRAME, "samples per frame is %d\n", mpg123_spf(Handle));

    memset(&Info, 0, sizeof(Info));
    ok(mpg123_info2(Handle, &Info) == MPG123_OK, "mpg123_info2 failed\n");
    ok(Info.version == MPG123_1_0, "MPEG version is %d\n", Info.version);
    ok(Info.layer == 3, "layer is %d\n", Info.layer);
    ok(Info.rate == 44100, "frame rate is %ld\n", Info.rate);
    ok(Info.mode == MPG123_M_MONO, "mode is %d\n", Info.mode);
    ok(Info.bitrate == 128, "bitrate is %d\n", Info.bitrate);
    ok(Info.framesize == MP3_FRAME_BYTES, "frame size is %d\n", Info.framesize);

    mpg123_delete(Handle);
}

static void
Test_DecodeSilence(void)
{
    DecodeSilence(MPG123_ENC_FLOAT_32, sizeof(float));
    DecodeSilence(MPG123_ENC_SIGNED_16, sizeof(short));
}

static DWORD WINAPI
NtoMTooLowRateThread(LPVOID Parameter)
{
    static unsigned char Out[MP3_SAMPLES_PER_FRAME * 2 * MP3_FRAME_COUNT];
    NTOM_RESULT *Result = Parameter;
    mpg123_handle *Handle;
    size_t Done;
    int Loop;

    Handle = mpg123_new(NULL, NULL);
    if (!Handle)
        return 1;

    mpg123_param2(Handle, MPG123_ADD_FLAGS, MPG123_QUIET, 0.0);
    Result->ReadResult = mpg123_param2(Handle, MPG123_FORCE_RATE, 1, 0.0);
    if (Result->ReadResult == MPG123_OK && mpg123_open_feed(Handle) == MPG123_OK &&
        mpg123_feed(Handle, SilentStream, sizeof(SilentStream)) == MPG123_OK)
    {
        for (Loop = 0; Loop < 16; Loop++)
        {
            Done = 0;
            Result->ReadResult = mpg123_read(Handle, Out, sizeof(Out), &Done);
            if (Result->ReadResult != MPG123_OK && Result->ReadResult != MPG123_NEW_FORMAT)
                break;
        }
        Result->ErrorCode = mpg123_errcode(Handle);
    }

    mpg123_delete(Handle);
    return 0;
}

static void
Test_NtoMTooLowRate(void)
{
    NTOM_RESULT Result = { MPG123_OK, MPG123_OK };
    HANDLE Thread;
    DWORD Wait;

    Thread = CreateThread(NULL, 0, NtoMTooLowRateThread, &Result, 0, NULL);
    ok(Thread != NULL, "CreateThread failed with %lu\n", GetLastError());
    if (!Thread)
        return;

    Wait = WaitForSingleObject(Thread, 10000);
    ok(Wait == WAIT_OBJECT_0, "decoding with a 1 Hz forced rate did not return (wait %lu)\n", Wait);
    if (Wait == WAIT_OBJECT_0)
    {
        ok(Result.ReadResult == MPG123_ERR, "mpg123_read returned %d\n", Result.ReadResult);
        ok(Result.ErrorCode == MPG123_BAD_RATE, "error code is %d\n", Result.ErrorCode);
    }
    else
    {
        TerminateThread(Thread, 0);
    }
    CloseHandle(Thread);
}

static void
Test_StoreUtf8(void)
{
    static const unsigned char Latin1[] = { 0xE9, 't', 0xE9 };
    static const char Utf8[] = "\xC3\xA9t\xC3\xA9";
    static const unsigned char Empty[1] = { 0 };
    mpg123_string String;
    int Ret;

    ok(mpg123_store_utf8_2(NULL, mpg123_text_utf8, Latin1, sizeof(Latin1)) == 0,
       "a NULL string was reported as stored\n");

    mpg123_init_string(&String);
    Ret = mpg123_store_utf8_2(&String, mpg123_text_latin1, Latin1, sizeof(Latin1));
    ok(Ret == 1, "Latin-1 conversion returned %d\n", Ret);
    ok(String.fill == sizeof(Utf8), "UTF-8 fill is %lu\n", (unsigned long)String.fill);
    ok(String.p != NULL && memcmp(String.p, Utf8, sizeof(Utf8)) == 0, "unexpected UTF-8 text\n");

    Ret = mpg123_store_utf8_2(&String, mpg123_text_utf8, NULL, 0);
    ok(Ret == 0, "a NULL source returned %d\n", Ret);
    ok(String.fill == 0, "fill after a NULL source is %lu\n", (unsigned long)String.fill);

    Ret = mpg123_store_utf8_2(&String, mpg123_text_icy, Empty, 0);
    ok(Ret == 0, "an empty ICY source returned %d\n", Ret);
    ok(String.fill == 0, "fill after an empty ICY source is %lu\n", (unsigned long)String.fill);

    mpg123_free_string(&String);
}

START_TEST(mpg123)
{
    BuildSilentStream();
    Test_Version();
    Test_Features();
    Test_Decoders();
    Test_DecodeSilence();
    Test_NtoMTooLowRate();
    Test_StoreUtf8();
}
