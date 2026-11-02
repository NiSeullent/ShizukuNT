/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test TrueType composite glyph chains of increasing depth
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include "precomp.h"

#define CHAIN_FONT_FAMILY L"LiberNT Composite Depth Test"
#define CHAIN_SHALLOW_DEPTH 2
#define CHAIN_MODERATE_DEPTH 16
#define CHAIN_DEEP_DEPTH 1000
#define CHAIN_NUM_GLYPHS (CHAIN_DEEP_DEPTH + 2)
#define CHAIN_UNITS_PER_EM 1000
#define CHAIN_ADVANCE 600
#define CHAIN_X_MIN 100
#define CHAIN_Y_MIN 0
#define CHAIN_X_MAX 500
#define CHAIN_Y_MAX 400
#define CHAIN_NUM_TABLES 10
#define SFNT_TAG(a, b, c, d) (((DWORD)(BYTE)(a) << 24) | ((DWORD)(BYTE)(b) << 16) | ((DWORD)(BYTE)(c) << 8) | (DWORD)(BYTE)(d))

typedef struct _SFNT_BUFFER
{
    BYTE *Data;
    SIZE_T Size;
    SIZE_T Capacity;
    BOOL Failed;
} SFNT_BUFFER;

typedef struct _SFNT_TABLE
{
    DWORD Tag;
    SFNT_BUFFER Body;
} SFNT_TABLE;

static void
SfntReserve(SFNT_BUFFER *Buffer, SIZE_T Extra)
{
    BYTE *NewData;
    SIZE_T NewCapacity;

    if (Buffer->Failed || Buffer->Size + Extra <= Buffer->Capacity)
        return;

    NewCapacity = Buffer->Capacity ? Buffer->Capacity : 256;
    while (NewCapacity < Buffer->Size + Extra)
        NewCapacity *= 2;

    NewData = realloc(Buffer->Data, NewCapacity);
    if (!NewData)
    {
        Buffer->Failed = TRUE;
        return;
    }

    Buffer->Data = NewData;
    Buffer->Capacity = NewCapacity;
}

static void
SfntPutU8(SFNT_BUFFER *Buffer, BYTE Value)
{
    SfntReserve(Buffer, 1);
    if (Buffer->Failed)
        return;
    Buffer->Data[Buffer->Size++] = Value;
}

static void
SfntPutU16(SFNT_BUFFER *Buffer, WORD Value)
{
    SfntPutU8(Buffer, (BYTE)(Value >> 8));
    SfntPutU8(Buffer, (BYTE)Value);
}

static void
SfntPutU32(SFNT_BUFFER *Buffer, DWORD Value)
{
    SfntPutU16(Buffer, (WORD)(Value >> 16));
    SfntPutU16(Buffer, (WORD)Value);
}

static void
SfntPutZeros(SFNT_BUFFER *Buffer, SIZE_T Count)
{
    while (Count--)
        SfntPutU8(Buffer, 0);
}

static void
SfntAlign4(SFNT_BUFFER *Buffer)
{
    while (Buffer->Size & 3)
        SfntPutU8(Buffer, 0);
}

static void
SfntPatchU32(SFNT_BUFFER *Buffer, SIZE_T Offset, DWORD Value)
{
    if (Buffer->Failed || Offset + 4 > Buffer->Size)
        return;
    Buffer->Data[Offset] = (BYTE)(Value >> 24);
    Buffer->Data[Offset + 1] = (BYTE)(Value >> 16);
    Buffer->Data[Offset + 2] = (BYTE)(Value >> 8);
    Buffer->Data[Offset + 3] = (BYTE)Value;
}

static DWORD
SfntChecksum(const BYTE *Data, SIZE_T Size)
{
    DWORD Sum = 0;
    SIZE_T i, j;

    for (i = 0; i < Size; i += 4)
    {
        DWORD Word = 0;

        for (j = 0; j < 4; j++)
            Word = (Word << 8) | ((i + j < Size) ? Data[i + j] : 0);
        Sum += Word;
    }

    return Sum;
}

static void
SfntPutUtf16Be(SFNT_BUFFER *Buffer, PCWSTR String)
{
    while (*String)
        SfntPutU16(Buffer, *String++);
}

static WORD
ChainGlyphForDepth(UINT Depth)
{
    return (WORD)(Depth + 1);
}

static void
BuildSimpleSquare(SFNT_BUFFER *Glyf)
{
    SfntPutU16(Glyf, 1);
    SfntPutU16(Glyf, CHAIN_X_MIN);
    SfntPutU16(Glyf, CHAIN_Y_MIN);
    SfntPutU16(Glyf, CHAIN_X_MAX);
    SfntPutU16(Glyf, CHAIN_Y_MAX);
    SfntPutU16(Glyf, 3);
    SfntPutU16(Glyf, 0);
    SfntPutU8(Glyf, 0x01);
    SfntPutU8(Glyf, 0x01);
    SfntPutU8(Glyf, 0x01);
    SfntPutU8(Glyf, 0x01);
    SfntPutU16(Glyf, CHAIN_X_MIN);
    SfntPutU16(Glyf, 0);
    SfntPutU16(Glyf, CHAIN_X_MAX - CHAIN_X_MIN);
    SfntPutU16(Glyf, 0);
    SfntPutU16(Glyf, CHAIN_Y_MIN);
    SfntPutU16(Glyf, CHAIN_Y_MAX - CHAIN_Y_MIN);
    SfntPutU16(Glyf, 0);
    SfntPutU16(Glyf, (WORD)(CHAIN_Y_MIN - CHAIN_Y_MAX));
    SfntAlign4(Glyf);
}

static void
BuildComposite(SFNT_BUFFER *Glyf, WORD Component)
{
    SfntPutU16(Glyf, 0xFFFF);
    SfntPutU16(Glyf, CHAIN_X_MIN);
    SfntPutU16(Glyf, CHAIN_Y_MIN);
    SfntPutU16(Glyf, CHAIN_X_MAX);
    SfntPutU16(Glyf, CHAIN_Y_MAX);
    SfntPutU16(Glyf, 0x0003);
    SfntPutU16(Glyf, Component);
    SfntPutU16(Glyf, 0);
    SfntPutU16(Glyf, 0);
    SfntAlign4(Glyf);
}

static void
BuildGlyfLoca(SFNT_BUFFER *Glyf, SFNT_BUFFER *Loca)
{
    UINT Glyph;

    for (Glyph = 0; Glyph < CHAIN_NUM_GLYPHS; Glyph++)
    {
        SfntPutU32(Loca, (DWORD)Glyf->Size);
        if (Glyph < 2)
            BuildSimpleSquare(Glyf);
        else
            BuildComposite(Glyf, (WORD)(Glyph - 1));
    }
    SfntPutU32(Loca, (DWORD)Glyf->Size);
}

static void
BuildCmap(SFNT_BUFFER *Cmap)
{
    static const WORD Chars[] = { L'A', L'B', L'C', L'D' };
    WORD Glyphs[ARRAYSIZE(Chars)];
    UINT i;

    Glyphs[0] = ChainGlyphForDepth(CHAIN_DEEP_DEPTH);
    Glyphs[1] = ChainGlyphForDepth(CHAIN_SHALLOW_DEPTH);
    Glyphs[2] = ChainGlyphForDepth(0);
    Glyphs[3] = ChainGlyphForDepth(CHAIN_MODERATE_DEPTH);

    SfntPutU16(Cmap, 0);
    SfntPutU16(Cmap, 1);
    SfntPutU16(Cmap, 3);
    SfntPutU16(Cmap, 1);
    SfntPutU32(Cmap, 12);

    SfntPutU16(Cmap, 4);
    SfntPutU16(Cmap, 16 + 8 * (ARRAYSIZE(Chars) + 1));
    SfntPutU16(Cmap, 0);
    SfntPutU16(Cmap, 2 * (ARRAYSIZE(Chars) + 1));
    SfntPutU16(Cmap, 8);
    SfntPutU16(Cmap, 2);
    SfntPutU16(Cmap, 2 * (ARRAYSIZE(Chars) + 1) - 8);
    for (i = 0; i < ARRAYSIZE(Chars); i++)
        SfntPutU16(Cmap, Chars[i]);
    SfntPutU16(Cmap, 0xFFFF);
    SfntPutU16(Cmap, 0);
    for (i = 0; i < ARRAYSIZE(Chars); i++)
        SfntPutU16(Cmap, Chars[i]);
    SfntPutU16(Cmap, 0xFFFF);
    for (i = 0; i < ARRAYSIZE(Chars); i++)
        SfntPutU16(Cmap, (WORD)(Glyphs[i] - Chars[i]));
    SfntPutU16(Cmap, 1);
    for (i = 0; i <= ARRAYSIZE(Chars); i++)
        SfntPutU16(Cmap, 0);
}

static void
BuildHead(SFNT_BUFFER *Head)
{
    SfntPutU16(Head, 1);
    SfntPutU16(Head, 0);
    SfntPutU32(Head, 0x00010000);
    SfntPutU32(Head, 0);
    SfntPutU32(Head, 0x5F0F3CF5);
    SfntPutU16(Head, 0x000B);
    SfntPutU16(Head, CHAIN_UNITS_PER_EM);
    SfntPutZeros(Head, 16);
    SfntPutU16(Head, CHAIN_X_MIN);
    SfntPutU16(Head, CHAIN_Y_MIN);
    SfntPutU16(Head, CHAIN_X_MAX);
    SfntPutU16(Head, CHAIN_Y_MAX);
    SfntPutU16(Head, 0);
    SfntPutU16(Head, 8);
    SfntPutU16(Head, 2);
    SfntPutU16(Head, 1);
    SfntPutU16(Head, 0);
}

static void
BuildHhea(SFNT_BUFFER *Hhea)
{
    SfntPutU16(Hhea, 1);
    SfntPutU16(Hhea, 0);
    SfntPutU16(Hhea, 800);
    SfntPutU16(Hhea, (WORD)-200);
    SfntPutU16(Hhea, 0);
    SfntPutU16(Hhea, CHAIN_ADVANCE);
    SfntPutU16(Hhea, CHAIN_X_MIN);
    SfntPutU16(Hhea, CHAIN_ADVANCE - CHAIN_X_MAX);
    SfntPutU16(Hhea, CHAIN_X_MAX);
    SfntPutU16(Hhea, 1);
    SfntPutU16(Hhea, 0);
    SfntPutU16(Hhea, 0);
    SfntPutZeros(Hhea, 8);
    SfntPutU16(Hhea, 0);
    SfntPutU16(Hhea, CHAIN_NUM_GLYPHS);
}

static void
BuildHmtx(SFNT_BUFFER *Hmtx)
{
    UINT Glyph;

    for (Glyph = 0; Glyph < CHAIN_NUM_GLYPHS; Glyph++)
    {
        SfntPutU16(Hmtx, CHAIN_ADVANCE);
        SfntPutU16(Hmtx, CHAIN_X_MIN);
    }
}

static void
BuildMaxp(SFNT_BUFFER *Maxp)
{
    SfntPutU32(Maxp, 0x00010000);
    SfntPutU16(Maxp, CHAIN_NUM_GLYPHS);
    SfntPutU16(Maxp, 4);
    SfntPutU16(Maxp, 1);
    SfntPutU16(Maxp, 4);
    SfntPutU16(Maxp, 1);
    SfntPutU16(Maxp, 2);
    SfntPutZeros(Maxp, 12);
    SfntPutU16(Maxp, 1);
    SfntPutU16(Maxp, CHAIN_DEEP_DEPTH);
}

static void
BuildName(SFNT_BUFFER *Name)
{
    static const struct
    {
        WORD NameId;
        PCWSTR String;
    } Records[] =
    {
        { 1, CHAIN_FONT_FAMILY },
        { 2, L"Regular" },
        { 3, CHAIN_FONT_FAMILY L" Regular" },
        { 4, CHAIN_FONT_FAMILY },
        { 6, L"LiberNTCompositeDepthTest-Regular" },
    };
    WORD Offset = 0;
    UINT i;

    SfntPutU16(Name, 0);
    SfntPutU16(Name, ARRAYSIZE(Records));
    SfntPutU16(Name, (WORD)(6 + 12 * ARRAYSIZE(Records)));
    for (i = 0; i < ARRAYSIZE(Records); i++)
    {
        WORD Length = (WORD)(wcslen(Records[i].String) * 2);

        SfntPutU16(Name, 3);
        SfntPutU16(Name, 1);
        SfntPutU16(Name, 0x0409);
        SfntPutU16(Name, Records[i].NameId);
        SfntPutU16(Name, Length);
        SfntPutU16(Name, Offset);
        Offset = (WORD)(Offset + Length);
    }
    for (i = 0; i < ARRAYSIZE(Records); i++)
        SfntPutUtf16Be(Name, Records[i].String);
}

static void
BuildOs2(SFNT_BUFFER *Os2)
{
    SfntPutU16(Os2, 4);
    SfntPutU16(Os2, CHAIN_ADVANCE);
    SfntPutU16(Os2, FW_NORMAL);
    SfntPutU16(Os2, 5);
    SfntPutU16(Os2, 0);
    SfntPutU16(Os2, 650);
    SfntPutU16(Os2, 600);
    SfntPutU16(Os2, 0);
    SfntPutU16(Os2, 75);
    SfntPutU16(Os2, 650);
    SfntPutU16(Os2, 600);
    SfntPutU16(Os2, 0);
    SfntPutU16(Os2, 350);
    SfntPutU16(Os2, 50);
    SfntPutU16(Os2, 250);
    SfntPutU16(Os2, 0);
    SfntPutZeros(Os2, 10);
    SfntPutU32(Os2, 1);
    SfntPutU32(Os2, 0);
    SfntPutU32(Os2, 0);
    SfntPutU32(Os2, 0);
    SfntPutU8(Os2, 'N');
    SfntPutU8(Os2, 'O');
    SfntPutU8(Os2, 'N');
    SfntPutU8(Os2, 'E');
    SfntPutU16(Os2, 0x0040);
    SfntPutU16(Os2, L'A');
    SfntPutU16(Os2, L'D');
    SfntPutU16(Os2, 800);
    SfntPutU16(Os2, (WORD)-200);
    SfntPutU16(Os2, 0);
    SfntPutU16(Os2, 800);
    SfntPutU16(Os2, 200);
    SfntPutU32(Os2, 1);
    SfntPutU32(Os2, 0);
    SfntPutU16(Os2, CHAIN_Y_MAX);
    SfntPutU16(Os2, CHAIN_Y_MAX);
    SfntPutU16(Os2, 0);
    SfntPutU16(Os2, L'A');
    SfntPutU16(Os2, 1);
}

static void
BuildPost(SFNT_BUFFER *Post)
{
    SfntPutU32(Post, 0x00030000);
    SfntPutU32(Post, 0);
    SfntPutU16(Post, (WORD)-100);
    SfntPutU16(Post, 50);
    SfntPutZeros(Post, 20);
}

static BYTE *
BuildCompositeChainFont(DWORD *FontSize)
{
    SFNT_TABLE Tables[CHAIN_NUM_TABLES];
    SFNT_BUFFER Font;
    SFNT_TABLE *Head = NULL;
    SIZE_T HeadOffset = 0;
    BOOL Failed = FALSE;
    UINT i;

    ZeroMemory(Tables, sizeof(Tables));
    ZeroMemory(&Font, sizeof(Font));

    Tables[0].Tag = SFNT_TAG('O', 'S', '/', '2');
    BuildOs2(&Tables[0].Body);
    Tables[1].Tag = SFNT_TAG('c', 'm', 'a', 'p');
    BuildCmap(&Tables[1].Body);
    Tables[2].Tag = SFNT_TAG('g', 'l', 'y', 'f');
    Tables[6].Tag = SFNT_TAG('l', 'o', 'c', 'a');
    BuildGlyfLoca(&Tables[2].Body, &Tables[6].Body);
    Tables[3].Tag = SFNT_TAG('h', 'e', 'a', 'd');
    BuildHead(&Tables[3].Body);
    Tables[4].Tag = SFNT_TAG('h', 'h', 'e', 'a');
    BuildHhea(&Tables[4].Body);
    Tables[5].Tag = SFNT_TAG('h', 'm', 't', 'x');
    BuildHmtx(&Tables[5].Body);
    Tables[7].Tag = SFNT_TAG('m', 'a', 'x', 'p');
    BuildMaxp(&Tables[7].Body);
    Tables[8].Tag = SFNT_TAG('n', 'a', 'm', 'e');
    BuildName(&Tables[8].Body);
    Tables[9].Tag = SFNT_TAG('p', 'o', 's', 't');
    BuildPost(&Tables[9].Body);

    SfntPutU32(&Font, 0x00010000);
    SfntPutU16(&Font, CHAIN_NUM_TABLES);
    SfntPutU16(&Font, 128);
    SfntPutU16(&Font, 3);
    SfntPutU16(&Font, CHAIN_NUM_TABLES * 16 - 128);

    {
        DWORD Offset = 12 + CHAIN_NUM_TABLES * 16;

        for (i = 0; i < CHAIN_NUM_TABLES; i++)
        {
            Failed |= Tables[i].Body.Failed;
            SfntPutU32(&Font, Tables[i].Tag);
            SfntPutU32(&Font, SfntChecksum(Tables[i].Body.Data, Tables[i].Body.Size));
            SfntPutU32(&Font, Offset);
            SfntPutU32(&Font, (DWORD)Tables[i].Body.Size);
            Offset += (DWORD)((Tables[i].Body.Size + 3) & ~(SIZE_T)3);
        }
    }

    for (i = 0; i < CHAIN_NUM_TABLES; i++)
    {
        if (Tables[i].Tag == SFNT_TAG('h', 'e', 'a', 'd'))
        {
            Head = &Tables[i];
            HeadOffset = Font.Size;
        }
        SfntReserve(&Font, Tables[i].Body.Size);
        if (!Font.Failed && Tables[i].Body.Size)
        {
            memcpy(Font.Data + Font.Size, Tables[i].Body.Data, Tables[i].Body.Size);
            Font.Size += Tables[i].Body.Size;
        }
        SfntAlign4(&Font);
        free(Tables[i].Body.Data);
    }

    if (Head)
        SfntPatchU32(&Font, HeadOffset + 8, 0xB1B0AFBA - SfntChecksum(Font.Data, Font.Size));

    if (Failed || Font.Failed || !Head)
    {
        free(Font.Data);
        return NULL;
    }

    *FontSize = (DWORD)Font.Size;
    return Font.Data;
}

static const MAT2 IdentityMatrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };

static BOOL
SameGlyphMetrics(const GLYPHMETRICS *Left, const GLYPHMETRICS *Right)
{
    return Left->gmBlackBoxX == Right->gmBlackBoxX &&
           Left->gmBlackBoxY == Right->gmBlackBoxY &&
           Left->gmptGlyphOrigin.x == Right->gmptGlyphOrigin.x &&
           Left->gmptGlyphOrigin.y == Right->gmptGlyphOrigin.y &&
           Left->gmCellIncX == Right->gmCellIncX &&
           Left->gmCellIncY == Right->gmCellIncY;
}

static void
TestResolvedComposite(HDC hdc, WCHAR Char, UINT Depth, const GLYPHMETRICS *Expected)
{
    GLYPHMETRICS gm;
    DWORD Ret;

    memset(&gm, 0xCC, sizeof(gm));
    Ret = GetGlyphOutlineW(hdc, Char, GGO_METRICS, &gm, 0, NULL, &IdentityMatrix);
    ok(Ret != GDI_ERROR, "Depth %u: GetGlyphOutlineW failed\n", Depth);
    if (Ret == GDI_ERROR)
        return;

    ok_int(gm.gmBlackBoxX, Expected->gmBlackBoxX);
    ok_int(gm.gmBlackBoxY, Expected->gmBlackBoxY);
    ok_long(gm.gmptGlyphOrigin.x, Expected->gmptGlyphOrigin.x);
    ok_long(gm.gmptGlyphOrigin.y, Expected->gmptGlyphOrigin.y);
    ok_int(gm.gmCellIncX, Expected->gmCellIncX);
    ok_int(gm.gmCellIncY, Expected->gmCellIncY);
}

static void
TestDeepComposite(HDC hdc, const GLYPHMETRICS *Simple)
{
    GLYPHMETRICS gm;
    DWORD Ret, Size;
    BYTE *Bits;

    memset(&gm, 0xCC, sizeof(gm));
    Ret = GetGlyphOutlineW(hdc, L'A', GGO_METRICS, &gm, 0, NULL, &IdentityMatrix);
    ok(Ret == GDI_ERROR || SameGlyphMetrics(&gm, Simple),
       "Depth %u: unexpected metrics %u x %u at (%ld, %ld)\n", CHAIN_DEEP_DEPTH,
       gm.gmBlackBoxX, gm.gmBlackBoxY, gm.gmptGlyphOrigin.x, gm.gmptGlyphOrigin.y);

    Size = GetGlyphOutlineW(hdc, L'A', GGO_GRAY8_BITMAP, &gm, 0, NULL, &IdentityMatrix);
    if (Size != GDI_ERROR && Size != 0)
    {
        Bits = malloc(Size);
        ok(Bits != NULL, "Out of memory\n");
        if (Bits)
        {
            Ret = GetGlyphOutlineW(hdc, L'A', GGO_GRAY8_BITMAP, &gm, Size, Bits, &IdentityMatrix);
            ok(Ret != GDI_ERROR, "Depth %u: bitmap query succeeded but rendering failed\n", CHAIN_DEEP_DEPTH);
            free(Bits);
        }
    }

    ExtTextOutW(hdc, 0, 0, 0, NULL, L"ABCD", 4, NULL);
}

START_TEST(CompositeGlyphDepth)
{
    BYTE *FontData;
    DWORD FontSize = 0, NumFonts = 0, Ret;
    HANDLE hFontRes;
    HDC hdc;
    HFONT hFont, hOldFont;
    HBITMAP hBitmap, hOldBitmap;
    LOGFONTW lf;
    WCHAR Face[LF_FACESIZE];
    GLYPHMETRICS Simple, After;

    FontData = BuildCompositeChainFont(&FontSize);
    ok(FontData != NULL, "Failed to build the test font\n");
    if (!FontData)
        return;

    hFontRes = AddFontMemResourceEx(FontData, FontSize, NULL, &NumFonts);
    ok(hFontRes != NULL, "AddFontMemResourceEx failed\n");
    if (!hFontRes)
    {
        free(FontData);
        return;
    }
    ok_long(NumFonts, 1);

    hdc = CreateCompatibleDC(NULL);
    ok(hdc != NULL, "CreateCompatibleDC failed\n");
    hBitmap = CreateCompatibleBitmap(hdc, 256, 128);
    ok(hBitmap != NULL, "CreateCompatibleBitmap failed\n");
    hOldBitmap = SelectObject(hdc, hBitmap);

    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -64;
    lf.lfWeight = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    StringCchCopyW(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), CHAIN_FONT_FAMILY);
    hFont = CreateFontIndirectW(&lf);
    ok(hFont != NULL, "CreateFontIndirectW failed\n");
    hOldFont = SelectObject(hdc, hFont);

    Face[0] = UNICODE_NULL;
    GetTextFaceW(hdc, ARRAYSIZE(Face), Face);
    ok(!wcscmp(Face, CHAIN_FONT_FAMILY), "Selected face is %s\n", wine_dbgstr_w(Face));
    if (wcscmp(Face, CHAIN_FONT_FAMILY))
    {
        skip("The test font was not selected\n");
        goto Cleanup;
    }

    memset(&Simple, 0xCC, sizeof(Simple));
    Ret = GetGlyphOutlineW(hdc, L'C', GGO_METRICS, &Simple, 0, NULL, &IdentityMatrix);
    ok(Ret != GDI_ERROR, "GetGlyphOutlineW failed for the simple glyph\n");
    if (Ret == GDI_ERROR)
        goto Cleanup;
    ok(Simple.gmBlackBoxX > 0 && Simple.gmBlackBoxY > 0,
       "Empty simple glyph %u x %u\n", Simple.gmBlackBoxX, Simple.gmBlackBoxY);

    TestResolvedComposite(hdc, L'B', CHAIN_SHALLOW_DEPTH, &Simple);
    TestResolvedComposite(hdc, L'D', CHAIN_MODERATE_DEPTH, &Simple);
    TestDeepComposite(hdc, &Simple);

    memset(&After, 0xCC, sizeof(After));
    Ret = GetGlyphOutlineW(hdc, L'C', GGO_METRICS, &After, 0, NULL, &IdentityMatrix);
    ok(Ret != GDI_ERROR, "GetGlyphOutlineW failed after the deep chain\n");
    ok(Ret == GDI_ERROR || SameGlyphMetrics(&After, &Simple),
       "Simple glyph metrics changed after the deep chain\n");

Cleanup:
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SelectObject(hdc, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    ok(RemoveFontMemResourceEx(hFontRes), "RemoveFontMemResourceEx failed\n");
    free(FontData);
}
