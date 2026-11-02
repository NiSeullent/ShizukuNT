/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for the bundled libpng library
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>
#include <string.h>
#include <png.h>

static void
Test_SetterAliasing(void)
{
    static const png_color Palette[4] =
    {
        { 0x10, 0x20, 0x30 },
        { 0x40, 0x50, 0x60 },
        { 0x70, 0x80, 0x90 },
        { 0xa0, 0xb0, 0xc0 },
    };
    static const png_byte Transparency[4] = { 0x00, 0x40, 0x80, 0xc0 };
    png_structp Png;
    png_infop Info;
    png_colorp CurrentPalette;
    png_bytep CurrentTransparency;
    png_color_16p TransparentColor;
    int PaletteCount;
    int TransparencyCount;
    png_uint_32 Valid;

    Png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    ok(Png != NULL, "png_create_read_struct failed\n");
    if (!Png)
        return;

    Info = png_create_info_struct(Png);
    ok(Info != NULL, "png_create_info_struct failed\n");
    if (!Info)
    {
        png_destroy_read_struct(&Png, NULL, NULL);
        return;
    }

    if (setjmp(png_jmpbuf(Png)))
    {
        ok(0, "libpng reported an error\n");
        png_destroy_read_struct(&Png, &Info, NULL);
        return;
    }

    png_set_IHDR(Png, Info, 1, 1, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_PLTE(Png, Info, Palette, 4);
    png_set_tRNS(Png, Info, Transparency, 4, NULL);

    Valid = png_get_PLTE(Png, Info, &CurrentPalette, &PaletteCount);
    ok(Valid == PNG_INFO_PLTE, "png_get_PLTE returned 0x%lx\n", (unsigned long)Valid);
    ok(PaletteCount == 4, "png_get_PLTE returned %d entries\n", PaletteCount);
    if (Valid == PNG_INFO_PLTE && PaletteCount == 4)
    {
        png_set_PLTE(Png, Info, CurrentPalette, PaletteCount);
        Valid = png_get_PLTE(Png, Info, &CurrentPalette, &PaletteCount);
        ok(Valid == PNG_INFO_PLTE, "png_get_PLTE returned 0x%lx after re-setting\n", (unsigned long)Valid);
        ok(PaletteCount == 4, "png_get_PLTE returned %d entries after re-setting\n", PaletteCount);
        ok(memcmp(CurrentPalette, Palette, sizeof(Palette)) == 0,
           "Palette changed when set from png_get_PLTE output: %02x%02x%02x %02x%02x%02x\n",
           CurrentPalette[0].red, CurrentPalette[0].green, CurrentPalette[0].blue,
           CurrentPalette[3].red, CurrentPalette[3].green, CurrentPalette[3].blue);
    }

    Valid = png_get_tRNS(Png, Info, &CurrentTransparency, &TransparencyCount, &TransparentColor);
    ok(Valid == PNG_INFO_tRNS, "png_get_tRNS returned 0x%lx\n", (unsigned long)Valid);
    ok(TransparencyCount == 4, "png_get_tRNS returned %d entries\n", TransparencyCount);
    if (Valid == PNG_INFO_tRNS && TransparencyCount == 4)
    {
        png_set_tRNS(Png, Info, CurrentTransparency, TransparencyCount, NULL);
        Valid = png_get_tRNS(Png, Info, &CurrentTransparency, &TransparencyCount, &TransparentColor);
        ok(Valid == PNG_INFO_tRNS, "png_get_tRNS returned 0x%lx after re-setting\n", (unsigned long)Valid);
        ok(TransparencyCount == 4, "png_get_tRNS returned %d entries after re-setting\n", TransparencyCount);
        ok(memcmp(CurrentTransparency, Transparency, sizeof(Transparency)) == 0,
           "Transparency changed when set from png_get_tRNS output: %02x %02x %02x %02x\n",
           CurrentTransparency[0], CurrentTransparency[1], CurrentTransparency[2], CurrentTransparency[3]);
    }

    png_destroy_read_struct(&Png, &Info, NULL);
}

START_TEST(libpng)
{
    Test_SetterAliasing();
}
