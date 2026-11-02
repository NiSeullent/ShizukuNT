/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the bundled minizip library
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>

#define MEMORY_ZIP_CAPACITY 0x40000
#define FIELD_LIMIT 0xffff

typedef struct _MEMORY_ZIP
{
    unsigned char *Data;
    ZPOS64_T Size;
    ZPOS64_T Position;
    int Error;
} MEMORY_ZIP;

static voidpf ZCALLBACK
MemoryOpen(voidpf Opaque, const void *FileName, int Mode)
{
    MEMORY_ZIP *Zip = Opaque;

    (void)FileName;
    if (Mode & ZLIB_FILEFUNC_MODE_CREATE)
        Zip->Size = 0;
    Zip->Position = 0;
    Zip->Error = 0;
    return Zip;
}

static uLong ZCALLBACK
MemoryRead(voidpf Opaque, voidpf Stream, void *Buffer, uLong Size)
{
    MEMORY_ZIP *Zip = Stream;
    ZPOS64_T Available;

    (void)Opaque;
    Available = (Zip->Position < Zip->Size) ? Zip->Size - Zip->Position : 0;
    if (Size > Available)
        Size = (uLong)Available;
    memcpy(Buffer, Zip->Data + Zip->Position, Size);
    Zip->Position += Size;
    return Size;
}

static uLong ZCALLBACK
MemoryWrite(voidpf Opaque, voidpf Stream, const void *Buffer, uLong Size)
{
    MEMORY_ZIP *Zip = Stream;

    (void)Opaque;
    if (Zip->Position > MEMORY_ZIP_CAPACITY || Size > MEMORY_ZIP_CAPACITY - Zip->Position)
    {
        Zip->Error = 1;
        return 0;
    }
    memcpy(Zip->Data + Zip->Position, Buffer, Size);
    Zip->Position += Size;
    if (Zip->Position > Zip->Size)
        Zip->Size = Zip->Position;
    return Size;
}

static ZPOS64_T ZCALLBACK
MemoryTell(voidpf Opaque, voidpf Stream)
{
    MEMORY_ZIP *Zip = Stream;

    (void)Opaque;
    return Zip->Position;
}

static long ZCALLBACK
MemorySeek(voidpf Opaque, voidpf Stream, ZPOS64_T Offset, int Origin)
{
    MEMORY_ZIP *Zip = Stream;
    ZPOS64_T Base;

    (void)Opaque;
    switch (Origin)
    {
        case ZLIB_FILEFUNC_SEEK_SET:
            Base = 0;
            break;
        case ZLIB_FILEFUNC_SEEK_CUR:
            Base = Zip->Position;
            break;
        case ZLIB_FILEFUNC_SEEK_END:
            Base = Zip->Size;
            break;
        default:
            return -1;
    }
    if (Offset > MEMORY_ZIP_CAPACITY - Base)
        return -1;
    Zip->Position = Base + Offset;
    return 0;
}

static int ZCALLBACK
MemoryClose(voidpf Opaque, voidpf Stream)
{
    (void)Opaque;
    (void)Stream;
    return 0;
}

static int ZCALLBACK
MemoryError(voidpf Opaque, voidpf Stream)
{
    MEMORY_ZIP *Zip = Stream;

    (void)Opaque;
    return Zip->Error;
}

static void
FillMemoryFileFunctions(zlib_filefunc64_def *FileFunctions, MEMORY_ZIP *Zip)
{
    FileFunctions->zopen64_file = MemoryOpen;
    FileFunctions->zread_file = MemoryRead;
    FileFunctions->zwrite_file = MemoryWrite;
    FileFunctions->ztell64_file = MemoryTell;
    FileFunctions->zseek64_file = MemorySeek;
    FileFunctions->zclose_file = MemoryClose;
    FileFunctions->zerror_file = MemoryError;
    FileFunctions->opaque = Zip;
}

static int
OpenEntry(zipFile ZipFile, const char *Name, const void *ExtraLocal, uInt ExtraLocalSize,
          const void *ExtraGlobal, uInt ExtraGlobalSize, const char *Comment)
{
    zip_fileinfo FileInfo;

    memset(&FileInfo, 0, sizeof(FileInfo));
    return zipOpenNewFileInZip4_64(ZipFile, Name, &FileInfo,
                                   ExtraLocal, ExtraLocalSize,
                                   ExtraGlobal, ExtraGlobalSize,
                                   Comment, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0,
                                   -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                   NULL, 0, 20, 0, 0);
}

static void
ExpectRejectedEntry(zipFile ZipFile, const char *Name, const void *ExtraLocal, uInt ExtraLocalSize,
                    const void *ExtraGlobal, uInt ExtraGlobalSize, const char *Comment,
                    const char *Description)
{
    int Status;

    Status = OpenEntry(ZipFile, Name, ExtraLocal, ExtraLocalSize, ExtraGlobal, ExtraGlobalSize, Comment);
    ok(Status == ZIP_PARAMERROR, "%s: zipOpenNewFileInZip4_64 returned %d\n", Description, Status);
    if (Status == ZIP_OK)
        zipCloseFileInZip(ZipFile);
}

static void
Test_HeaderFieldLimits(void)
{
    static const char Content[] = "Hello, LiberNT";
    static const char EntryName[] = "hello.txt";
    MEMORY_ZIP Zip;
    zlib_filefunc64_def FileFunctions;
    zipFile ZipFile;
    unzFile UnzipFile;
    unz_global_info64 GlobalInfo;
    char *LongText;
    unsigned char *LongExtra;
    char Output[sizeof(Content)];
    int Status;

    memset(&Zip, 0, sizeof(Zip));
    Zip.Data = calloc(1, MEMORY_ZIP_CAPACITY);
    LongText = malloc(FIELD_LIMIT + 2);
    LongExtra = calloc(1, FIELD_LIMIT + 1);
    if (!Zip.Data || !LongText || !LongExtra)
    {
        skip("Out of memory\n");
        goto Cleanup;
    }
    memset(LongText, 'a', FIELD_LIMIT + 1);
    LongText[FIELD_LIMIT + 1] = '\0';

    FillMemoryFileFunctions(&FileFunctions, &Zip);
    ZipFile = zipOpen2_64("memory.zip", APPEND_STATUS_CREATE, NULL, &FileFunctions);
    ok(ZipFile != NULL, "zipOpen2_64 failed\n");
    if (!ZipFile)
        goto Cleanup;

    ExpectRejectedEntry(ZipFile, LongText, NULL, 0, NULL, 0, NULL, "65536-byte name");
    ExpectRejectedEntry(ZipFile, EntryName, NULL, 0, NULL, 0, LongText, "65536-byte comment");
    ExpectRejectedEntry(ZipFile, EntryName, LongExtra, FIELD_LIMIT + 1, NULL, 0, NULL, "65536-byte local extra field");
    ExpectRejectedEntry(ZipFile, EntryName, NULL, 0, LongExtra, FIELD_LIMIT + 1, NULL, "65536-byte global extra field");

    Status = OpenEntry(ZipFile, EntryName, NULL, 0, NULL, 0, NULL);
    ok(Status == ZIP_OK, "zipOpenNewFileInZip4_64 returned %d\n", Status);
    if (Status == ZIP_OK)
    {
        Status = zipWriteInFileInZip(ZipFile, Content, sizeof(Content));
        ok(Status == ZIP_OK, "zipWriteInFileInZip returned %d\n", Status);
        Status = zipCloseFileInZip(ZipFile);
        ok(Status == ZIP_OK, "zipCloseFileInZip returned %d\n", Status);
    }
    Status = zipClose(ZipFile, NULL);
    ok(Status == ZIP_OK, "zipClose returned %d\n", Status);

    UnzipFile = unzOpen2_64("memory.zip", &FileFunctions);
    ok(UnzipFile != NULL, "unzOpen2_64 failed\n");
    if (!UnzipFile)
        goto Cleanup;

    Status = unzGetGlobalInfo64(UnzipFile, &GlobalInfo);
    ok(Status == UNZ_OK, "unzGetGlobalInfo64 returned %d\n", Status);
    ok(GlobalInfo.number_entry == 1, "Archive has %lu entries\n", (unsigned long)GlobalInfo.number_entry);

    Status = unzLocateFile(UnzipFile, EntryName, 0);
    ok(Status == UNZ_OK, "unzLocateFile returned %d\n", Status);
    if (Status == UNZ_OK)
    {
        Status = unzOpenCurrentFile(UnzipFile);
        ok(Status == UNZ_OK, "unzOpenCurrentFile returned %d\n", Status);
        if (Status == UNZ_OK)
        {
            memset(Output, 0, sizeof(Output));
            Status = unzReadCurrentFile(UnzipFile, Output, sizeof(Output));
            ok(Status == (int)sizeof(Content), "unzReadCurrentFile returned %d\n", Status);
            ok(memcmp(Output, Content, sizeof(Content)) == 0, "Entry content differs\n");
            Status = unzCloseCurrentFile(UnzipFile);
            ok(Status == UNZ_OK, "unzCloseCurrentFile returned %d\n", Status);
        }
    }
    unzClose(UnzipFile);

Cleanup:
    free(LongExtra);
    free(LongText);
    free(Zip.Data);
}

static void
Test_DefaultFileFunctions(void)
{
    static const char Content[] = "LiberNT default minizip file functions";
    static const char EntryName[] = "default.txt";
    char TempPath[MAX_PATH];
    char ZipPath[MAX_PATH];
    char Output[sizeof(Content)];
    zipFile ZipFile;
    unzFile UnzipFile;
    int Status;

    if (!GetTempPathA(sizeof(TempPath), TempPath) || !GetTempFileNameA(TempPath, "mz", 0, ZipPath))
    {
        skip("No temporary file available\n");
        return;
    }

    ZipFile = zipOpen64(ZipPath, APPEND_STATUS_CREATE);
    ok(ZipFile != NULL, "zipOpen64 failed for '%s'\n", ZipPath);
    if (!ZipFile)
        goto Cleanup;

    Status = OpenEntry(ZipFile, EntryName, NULL, 0, NULL, 0, NULL);
    ok(Status == ZIP_OK, "zipOpenNewFileInZip4_64 returned %d\n", Status);
    if (Status == ZIP_OK)
    {
        Status = zipWriteInFileInZip(ZipFile, Content, sizeof(Content));
        ok(Status == ZIP_OK, "zipWriteInFileInZip returned %d\n", Status);
        Status = zipCloseFileInZip(ZipFile);
        ok(Status == ZIP_OK, "zipCloseFileInZip returned %d\n", Status);
    }
    Status = zipClose(ZipFile, NULL);
    ok(Status == ZIP_OK, "zipClose returned %d\n", Status);

    UnzipFile = unzOpen64(ZipPath);
    ok(UnzipFile != NULL, "unzOpen64 failed for '%s'\n", ZipPath);
    if (!UnzipFile)
        goto Cleanup;

    Status = unzLocateFile(UnzipFile, EntryName, 0);
    ok(Status == UNZ_OK, "unzLocateFile returned %d\n", Status);
    if (Status == UNZ_OK && unzOpenCurrentFile(UnzipFile) == UNZ_OK)
    {
        memset(Output, 0, sizeof(Output));
        Status = unzReadCurrentFile(UnzipFile, Output, sizeof(Output));
        ok(Status == (int)sizeof(Content), "unzReadCurrentFile returned %d\n", Status);
        ok(memcmp(Output, Content, sizeof(Content)) == 0, "Entry content differs\n");
        unzCloseCurrentFile(UnzipFile);
    }
    unzClose(UnzipFile);

Cleanup:
    DeleteFileA(ZipPath);
}

START_TEST(minizip)
{
    Test_HeaderFieldLimits();
    Test_DefaultFileFunctions();
}
