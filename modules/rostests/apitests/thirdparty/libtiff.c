/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the bundled libtiff library
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

#define IFD_OFFSET 8
#define IFD_ENTRY_COUNT 10
#define PIXEL_OFFSET (IFD_OFFSET + 2 + IFD_ENTRY_COUNT * 12 + 4)
#define PIXEL_SIZE 6

typedef struct _MEMORY_TIFF
{
    const unsigned char *Data;
    toff_t Size;
    toff_t Position;
    BOOL FailReads;
} MEMORY_TIFF;

static char LastError[512];
static char LastWarning[512];

static void
RecordError(const char *Module, const char *Format, va_list Arguments)
{
    (void)Module;
    _vsnprintf(LastError, sizeof(LastError) - 1, Format, Arguments);
    LastError[sizeof(LastError) - 1] = '\0';
}

static void
RecordWarning(const char *Module, const char *Format, va_list Arguments)
{
    (void)Module;
    _vsnprintf(LastWarning, sizeof(LastWarning) - 1, Format, Arguments);
    LastWarning[sizeof(LastWarning) - 1] = '\0';
}

static tmsize_t
MemoryRead(thandle_t Handle, void *Buffer, tmsize_t Size)
{
    MEMORY_TIFF *Tiff = (MEMORY_TIFF *)Handle;
    toff_t Available;

    if (Tiff->FailReads)
        return -1;
    Available = (Tiff->Position < Tiff->Size) ? Tiff->Size - Tiff->Position : 0;
    if ((toff_t)Size > Available)
        Size = (tmsize_t)Available;
    memcpy(Buffer, Tiff->Data + Tiff->Position, (size_t)Size);
    Tiff->Position += Size;
    return Size;
}

static tmsize_t
MemoryWrite(thandle_t Handle, void *Buffer, tmsize_t Size)
{
    (void)Handle;
    (void)Buffer;
    (void)Size;
    return -1;
}

static toff_t
MemorySeek(thandle_t Handle, toff_t Offset, int Origin)
{
    MEMORY_TIFF *Tiff = (MEMORY_TIFF *)Handle;

    switch (Origin)
    {
        case SEEK_SET:
            Tiff->Position = Offset;
            break;
        case SEEK_CUR:
            Tiff->Position += Offset;
            break;
        case SEEK_END:
            Tiff->Position = Tiff->Size + Offset;
            break;
        default:
            return (toff_t)-1;
    }
    return Tiff->Position;
}

static int
MemoryClose(thandle_t Handle)
{
    (void)Handle;
    return 0;
}

static toff_t
MemorySize(thandle_t Handle)
{
    return ((MEMORY_TIFF *)Handle)->Size;
}

static int
MemoryMap(thandle_t Handle, void **Base, toff_t *Size)
{
    (void)Handle;
    (void)Base;
    (void)Size;
    return 0;
}

static void
MemoryUnmap(thandle_t Handle, void *Base, toff_t Size)
{
    (void)Handle;
    (void)Base;
    (void)Size;
}

static unsigned char *
PutEntry(unsigned char *Entry, unsigned short Tag, unsigned short Type,
         unsigned int Count, unsigned int Value)
{
    Entry[0] = (unsigned char)Tag;
    Entry[1] = (unsigned char)(Tag >> 8);
    Entry[2] = (unsigned char)Type;
    Entry[3] = (unsigned char)(Type >> 8);
    Entry[4] = (unsigned char)Count;
    Entry[5] = (unsigned char)(Count >> 8);
    Entry[6] = (unsigned char)(Count >> 16);
    Entry[7] = (unsigned char)(Count >> 24);
    Entry[8] = (unsigned char)Value;
    Entry[9] = (unsigned char)(Value >> 8);
    Entry[10] = (unsigned char)(Value >> 16);
    Entry[11] = (unsigned char)(Value >> 24);
    return Entry + 12;
}

static void
BuildRgbTiff(unsigned char *File, unsigned int BitsPerSampleCount)
{
    static const unsigned char Pixels[PIXEL_SIZE] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    unsigned char *Entry;

    memset(File, 0, PIXEL_OFFSET + PIXEL_SIZE);
    File[0] = 'I';
    File[1] = 'I';
    File[2] = 42;
    File[4] = IFD_OFFSET;
    File[IFD_OFFSET] = IFD_ENTRY_COUNT;

    Entry = File + IFD_OFFSET + 2;
    Entry = PutEntry(Entry, TIFFTAG_IMAGEWIDTH, TIFF_SHORT, 1, 2);
    Entry = PutEntry(Entry, TIFFTAG_IMAGELENGTH, TIFF_SHORT, 1, 1);
    Entry = PutEntry(Entry, TIFFTAG_BITSPERSAMPLE, TIFF_SHORT, BitsPerSampleCount, 0x00080008);
    Entry = PutEntry(Entry, TIFFTAG_COMPRESSION, TIFF_SHORT, 1, COMPRESSION_NONE);
    Entry = PutEntry(Entry, TIFFTAG_PHOTOMETRIC, TIFF_SHORT, 1, PHOTOMETRIC_RGB);
    Entry = PutEntry(Entry, TIFFTAG_STRIPOFFSETS, TIFF_LONG, 1, PIXEL_OFFSET);
    Entry = PutEntry(Entry, TIFFTAG_SAMPLESPERPIXEL, TIFF_SHORT, 1, 3);
    Entry = PutEntry(Entry, TIFFTAG_ROWSPERSTRIP, TIFF_SHORT, 1, 1);
    Entry = PutEntry(Entry, TIFFTAG_STRIPBYTECOUNTS, TIFF_LONG, 1, PIXEL_SIZE);
    PutEntry(Entry, TIFFTAG_PLANARCONFIG, TIFF_SHORT, 1, PLANARCONFIG_CONTIG);

    memcpy(File + PIXEL_OFFSET, Pixels, PIXEL_SIZE);
}

static TIFF *
OpenMemoryTiff(MEMORY_TIFF *Tiff, const unsigned char *File)
{
    memset(Tiff, 0, sizeof(*Tiff));
    Tiff->Data = File;
    Tiff->Size = PIXEL_OFFSET + PIXEL_SIZE;
    return TIFFClientOpen("memory.tif", "rm", (thandle_t)Tiff, MemoryRead, MemoryWrite,
                          MemorySeek, MemoryClose, MemorySize, MemoryMap, MemoryUnmap);
}

static void
Test_ReadStrip(void)
{
    unsigned char File[PIXEL_OFFSET + PIXEL_SIZE];
    unsigned char Strip[PIXEL_SIZE];
    MEMORY_TIFF Memory;
    TIFF *Tiff;
    uint32_t Width = 0;
    uint16_t BitsPerSample = 0, SamplesPerPixel = 0;
    tmsize_t Read;

    BuildRgbTiff(File, 1);
    Tiff = OpenMemoryTiff(&Memory, File);
    ok(Tiff != NULL, "TIFFClientOpen failed: %s\n", LastError);
    if (!Tiff)
        return;

    ok(TIFFGetField(Tiff, TIFFTAG_IMAGEWIDTH, &Width) == 1, "TIFFGetField(ImageWidth) failed\n");
    ok(Width == 2, "ImageWidth is %u\n", Width);
    ok(TIFFGetField(Tiff, TIFFTAG_BITSPERSAMPLE, &BitsPerSample) == 1, "TIFFGetField(BitsPerSample) failed\n");
    ok(BitsPerSample == 8, "BitsPerSample is %u\n", BitsPerSample);
    ok(TIFFGetField(Tiff, TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel) == 1, "TIFFGetField(SamplesPerPixel) failed\n");
    ok(SamplesPerPixel == 3, "SamplesPerPixel is %u\n", SamplesPerPixel);

    memset(Strip, 0, sizeof(Strip));
    Read = TIFFReadEncodedStrip(Tiff, 0, Strip, (tmsize_t)-1);
    ok(Read == PIXEL_SIZE, "TIFFReadEncodedStrip returned %ld\n", (long)Read);
    ok(memcmp(Strip, File + PIXEL_OFFSET, PIXEL_SIZE) == 0, "Strip data differs\n");

    TIFFClose(Tiff);
}

static void
Test_ShortPerSampleCount(void)
{
    unsigned char File[PIXEL_OFFSET + PIXEL_SIZE];
    MEMORY_TIFF Memory;
    TIFF *Tiff;
    uint16_t BitsPerSample = 0;

    BuildRgbTiff(File, 2);
    LastWarning[0] = '\0';
    Tiff = OpenMemoryTiff(&Memory, File);
    ok(Tiff != NULL, "TIFFClientOpen failed with two BitsPerSample values: %s\n", LastError);
    if (!Tiff)
        return;

    ok(TIFFGetField(Tiff, TIFFTAG_BITSPERSAMPLE, &BitsPerSample) == 1, "TIFFGetField(BitsPerSample) failed\n");
    ok(BitsPerSample == 8, "BitsPerSample is %u\n", BitsPerSample);
    ok(strstr(LastWarning, "BitsPerSample") != NULL, "Unexpected warning '%s'\n", LastWarning);

    TIFFClose(Tiff);
}

static void
Test_FailingReadProc(void)
{
    unsigned char File[PIXEL_OFFSET + PIXEL_SIZE];
    unsigned char Strip[PIXEL_SIZE];
    MEMORY_TIFF Memory;
    TIFF *Tiff;
    tmsize_t Read;

    BuildRgbTiff(File, 1);
    Tiff = OpenMemoryTiff(&Memory, File);
    ok(Tiff != NULL, "TIFFClientOpen failed: %s\n", LastError);
    if (!Tiff)
        return;

    Memory.FailReads = TRUE;
    LastError[0] = '\0';
    Read = TIFFReadEncodedStrip(Tiff, 0, Strip, (tmsize_t)-1);
    ok(Read == -1, "TIFFReadEncodedStrip returned %ld\n", (long)Read);
    ok(strstr(LastError, "got 0 bytes") != NULL, "Unexpected error '%s'\n", LastError);

    Memory.FailReads = FALSE;
    memset(Strip, 0, sizeof(Strip));
    Read = TIFFReadEncodedStrip(Tiff, 0, Strip, (tmsize_t)-1);
    ok(Read == PIXEL_SIZE, "TIFFReadEncodedStrip returned %ld after a failed read\n", (long)Read);
    ok(memcmp(Strip, File + PIXEL_OFFSET, PIXEL_SIZE) == 0, "Strip data differs after a failed read\n");

    TIFFClose(Tiff);
}

START_TEST(libtiff)
{
    TIFFErrorHandler PreviousError;
    TIFFErrorHandler PreviousWarning;

    PreviousError = TIFFSetErrorHandler(RecordError);
    PreviousWarning = TIFFSetWarningHandler(RecordWarning);

    Test_ReadStrip();
    Test_ShortPerSampleCount();
    Test_FailingReadProc();

    TIFFSetWarningHandler(PreviousWarning);
    TIFFSetErrorHandler(PreviousError);
}
