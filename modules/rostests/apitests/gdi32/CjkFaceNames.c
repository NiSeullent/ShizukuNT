/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test the fonts GDI selects for requested and CJK face names
 * COPYRIGHT:   Copyright 2026 mikuta0407 <mikuta0407@mikuta0407.net>
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include "precomp.h"
#include <strsafe.h>

static int CALLBACK FindFaceProc(const LOGFONTW *plf, const TEXTMETRICW *ptm, DWORD dwType, LPARAM lParam)
{
    *(BYTE *)lParam = ptm->tmPitchAndFamily | 0x80;
    return 0;
}

static BOOL IsProportionalFace(HDC hDC, PCWSTR pszFace)
{
    LOGFONTW lf;
    BYTE bPitch = 0;

    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    StringCchCopyW(lf.lfFaceName, _countof(lf.lfFaceName), pszFace);
    EnumFontFamiliesExW(hDC, &lf, FindFaceProc, (LPARAM)&bPitch, 0);
    return (bPitch & 0x80) && (bPitch & TMPF_FIXED_PITCH);
}

static HFONT CreateFaceFont(PCWSTR pszFace, BYTE bCharSet, BYTE bPitchAndFamily)
{
    return CreateFontW(-32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, bCharSet, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, bPitchAndFamily, pszFace);
}

static void TestFixedPitchRequest(HDC hDC)
{
    static const PCWSTR s_apszFaces[] =
    {
        L"Arial", L"Times New Roman", L"Tahoma", L"Comic Sans MS", L"Segoe UI",
        L"Noto Sans JP", L"Noto Sans SC", L"Yu Gothic UI", L"Meiryo"
    };
    TEXTMETRICW tm;
    HFONT hFont, hOldFont;
    UINT i, cTested = 0;

    for (i = 0; i < _countof(s_apszFaces); i++)
    {
        if (!IsProportionalFace(hDC, s_apszFaces[i]))
            continue;
        cTested++;
        hFont = CreateFaceFont(s_apszFaces[i], DEFAULT_CHARSET, FIXED_PITCH | FF_MODERN);
        hOldFont = SelectObject(hDC, hFont);
        GetTextMetricsW(hDC, &tm);
        ok(tm.tmPitchAndFamily & TMPF_FIXED_PITCH, "%ls was replaced by a fixed pitch font\n", s_apszFaces[i]);
        SelectObject(hDC, hOldFont);
        DeleteObject(hFont);
    }
    ok(cTested != 0, "no proportional face to test\n");

    hFont = CreateFaceFont(L"LiberNT No Such Face", DEFAULT_CHARSET, FIXED_PITCH | FF_MODERN);
    hOldFont = SelectObject(hDC, hFont);
    GetTextMetricsW(hDC, &tm);
    ok(!(tm.tmPitchAndFamily & TMPF_FIXED_PITCH), "an unknown face gave a proportional font\n");
    SelectObject(hDC, hOldFont);
    DeleteObject(hFont);
}

static void TestCjkFaces(HDC hDC)
{
    static const PCWSTR s_apszFaces[] =
    {
        L"MS UI Gothic", L"MS Gothic", L"MS PGothic", L"MS Mincho", L"MS PMincho", L"Meiryo", L"Meiryo UI",
        L"Yu Gothic", L"Yu Gothic UI", L"SimSun", L"NSimSun", L"Microsoft YaHei", L"Microsoft YaHei UI"
    };
    WORD awGlyphs[3];
    HFONT hFont, hOldFont;
    UINT i;

    for (i = 0; i < _countof(s_apszFaces); i++)
    {
        hFont = CreateFaceFont(s_apszFaces[i], DEFAULT_CHARSET, DEFAULT_PITCH | FF_DONTCARE);
        hOldFont = SelectObject(hDC, hFont);
        ok_long(GetGlyphIndicesW(hDC, L"\x65e5\x672c\x8a9e", 3, awGlyphs, GGI_MARK_NONEXISTING_GLYPHS), 3);
        ok(awGlyphs[0] != 0xFFFF && awGlyphs[1] != 0xFFFF && awGlyphs[2] != 0xFFFF,
           "%ls has no glyph for the kanji\n", s_apszFaces[i]);
        SelectObject(hDC, hOldFont);
        DeleteObject(hFont);
    }
}

static int CALLBACK CountFaceProc(const LOGFONTW *plf, const TEXTMETRICW *ptm, DWORD dwType, LPARAM lParam)
{
    (*(UINT *)lParam)++;
    return 1;
}

static void TestKoreanFaces(HDC hDC)
{
    static const struct
    {
        PCWSTR pszFace;
        BOOL bFixed;
    } s_aFaces[] =
    {
        { L"Malgun Gothic", FALSE }, { L"\xb9d1\xc740 \xace0\xb515", FALSE },
        { L"Gulim", FALSE }, { L"\xad74\xb9bc", FALSE },
        { L"GulimChe", TRUE }, { L"\xad74\xb9bc\xccb4", TRUE },
        { L"Dotum", FALSE }, { L"\xb3cb\xc6c0", FALSE },
        { L"DotumChe", TRUE }, { L"\xb3cb\xc6c0\xccb4", TRUE },
        { L"Batang", FALSE }, { L"\xbc14\xd0d5", FALSE },
        { L"BatangChe", TRUE }, { L"\xbc14\xd0d5\xccb4", TRUE },
        { L"Gungsuh", FALSE }, { L"\xad81\xc11c", FALSE },
        { L"GungsuhChe", TRUE }, { L"\xad81\xc11c\xccb4", TRUE },
        { L"MS Shell Dlg", FALSE }, { L"MS Shell Dlg 2", FALSE }
    };
    WORD awGlyphs[2];
    TEXTMETRICW tm;
    LOGFONTW lf;
    HFONT hFont, hOldFont;
    UINT i, cFaces = 0;

    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = HANGUL_CHARSET;
    EnumFontFamiliesExW(hDC, &lf, CountFaceProc, (LPARAM)&cFaces, 0);
    ok(cFaces != 0, "no font enumerates the Hangul charset\n");

    for (i = 0; i < _countof(s_aFaces); i++)
    {
        hFont = CreateFaceFont(s_aFaces[i].pszFace, HANGUL_CHARSET, DEFAULT_PITCH | FF_DONTCARE);
        hOldFont = SelectObject(hDC, hFont);
        ok_long(GetGlyphIndicesW(hDC, L"\xd55c\xae00", 2, awGlyphs, GGI_MARK_NONEXISTING_GLYPHS), 2);
        ok(awGlyphs[0] != 0xFFFF && awGlyphs[1] != 0xFFFF, "%ls has no glyph for the Hangul syllables\n",
           s_aFaces[i].pszFace);
        ok(GetTextCharset(hDC) == HANGUL_CHARSET, "%ls selected charset %d\n", s_aFaces[i].pszFace,
           GetTextCharset(hDC));
        ok(GetTextMetricsW(hDC, &tm), "GetTextMetricsW failed for %ls\n", s_aFaces[i].pszFace);
        ok(((tm.tmPitchAndFamily & TMPF_FIXED_PITCH) == 0) == s_aFaces[i].bFixed, "%ls has pitch and family 0x%02x\n",
           s_aFaces[i].pszFace, tm.tmPitchAndFamily);
        SelectObject(hDC, hOldFont);
        DeleteObject(hFont);
    }
}

static void DrawChar(HDC hDC, HFONT hFont, WCHAR wch)
{
    HFONT hOldFont;
    RECT rc = { 0, 0, 64, 64 };

    FillRect(hDC, &rc, GetStockObject(WHITE_BRUSH));
    hOldFont = SelectObject(hDC, hFont);
    TextOutW(hDC, 8, 8, &wch, 1);
    SelectObject(hDC, hOldFont);
    GdiFlush();
}

static void TestLinkedDrawing(void)
{
    static const PCWSTR s_apszFaces[] = { L"Tahoma", L"MS Shell Dlg", L"Microsoft Sans Serif", L"Arial", L"Courier New" };
    static const BYTE s_abCharSets[] = { DEFAULT_CHARSET, ANSI_CHARSET };
    static const WCHAR s_awPairs[][2] = { { 0x65e5, 0x672c }, { 0xd55c, 0xae00 } };
    BITMAPINFO bmi;
    BYTE *pBits, *pFirst;
    HBITMAP hBitmap, hOldBitmap;
    HFONT hFont;
    HDC hDC;
    UINT i, j, k;
    const SIZE_T cbBits = 64 * 64 * 4;

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = 64;
    bmi.bmiHeader.biHeight = -64;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    hDC = CreateCompatibleDC(NULL);
    hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);
    hOldBitmap = SelectObject(hDC, hBitmap);
    pFirst = HeapAlloc(GetProcessHeap(), 0, cbBits);

    for (i = 0; i < _countof(s_apszFaces); i++)
    {
        for (j = 0; j < _countof(s_abCharSets); j++)
        {
            hFont = CreateFaceFont(s_apszFaces[i], s_abCharSets[j], DEFAULT_PITCH | FF_DONTCARE);
            for (k = 0; k < _countof(s_awPairs); k++)
            {
                DrawChar(hDC, hFont, s_awPairs[k][0]);
                CopyMemory(pFirst, pBits, cbBits);
                DrawChar(hDC, hFont, s_awPairs[k][1]);
                ok(memcmp(pFirst, pBits, cbBits) != 0, "%ls with charset %u draws U+%04X and U+%04X as boxes\n",
                   s_apszFaces[i], s_abCharSets[j], s_awPairs[k][0], s_awPairs[k][1]);
            }
            DeleteObject(hFont);
        }
    }

    HeapFree(GetProcessHeap(), 0, pFirst);
    SelectObject(hDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hDC);
}

START_TEST(CjkFaceNames)
{
    HDC hDC = CreateCompatibleDC(NULL);

    TestFixedPitchRequest(hDC);
    TestCjkFaces(hDC);
    TestKoreanFaces(hDC);
    DeleteDC(hDC);
    TestLinkedDrawing();
}
