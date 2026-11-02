/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Test the scan codes and characters of the Korean keyboard layout
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include "precomp.h"

#define KOREAN_KEYBOARD_TYPE 8

static void TestVscToVk(HKL hkl, UINT vsc, UINT expect)
{
    UINT vk = MapVirtualKeyExW(vsc, MAPVK_VSC_TO_VK_EX, hkl);
    ok(vk == expect, "vsc %04x: got vk %02x, expected %02x\n", vsc, vk, expect);
}

static void TestVkToVsc(HKL hkl, UINT vk, UINT expect)
{
    UINT vsc = MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC_EX, hkl);
    ok(vsc == expect, "vk %02x: got vsc %04x, expected %04x\n", vk, vsc, expect);
}

static void TestChar(HKL hkl, UINT vk, BOOL shift, BOOL ctrl, const WCHAR *expect, int len)
{
    BYTE state[256] = { 0 };
    WCHAR buffer[8] = { 0 };
    int ret;

    if (shift)
        state[VK_SHIFT] = state[VK_LSHIFT] = 0x80;
    if (ctrl)
        state[VK_CONTROL] = state[VK_LCONTROL] = 0x80;

    ret = ToUnicodeEx(vk, MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC, hkl), state, buffer, ARRAYSIZE(buffer), 0, hkl);
    ok(ret == len, "vk %02x shift %d ctrl %d: got %d characters, expected %d\n", vk, shift, ctrl, ret, len);
    if (ret > 0 && len > 0)
    {
        ok(!memcmp(buffer, expect, len * sizeof(WCHAR)), "vk %02x shift %d ctrl %d: got %04x, expected %04x\n",
           vk, shift, ctrl, buffer[0], expect[0]);
    }
}

static void TestHangulHanjaKeys(HKL hkl)
{
    int type = GetKeyboardType(0);
    int subtype = GetKeyboardType(1);

    if (type != KOREAN_KEYBOARD_TYPE)
    {
        TestVscToVk(hkl, 0x72, VK_HANGUL);
        TestVscToVk(hkl, 0x71, VK_HANJA);
        TestVkToVsc(hkl, VK_HANGUL, 0x72);
        TestVkToVsc(hkl, VK_HANJA, 0x71);
        TestVscToVk(hkl, 0xE038, VK_RMENU);
        TestVscToVk(hkl, 0xE01D, VK_RCONTROL);
        return;
    }

    switch (subtype)
    {
        case 3:
            TestVscToVk(hkl, 0xE038, VK_HANGUL);
            TestVscToVk(hkl, 0xE01D, VK_HANJA);
            TestVscToVk(hkl, 0x72, 0);
            break;

        case 4:
            TestVscToVk(hkl, 0xE01D, VK_HANGUL);
            TestVscToVk(hkl, 0xE038, VK_HANJA);
            break;

        default:
            skip("Korean keyboard subtype %d has no documented right Alt/Ctrl mapping\n", subtype);
            break;
    }
}

START_TEST(KbdKorean)
{
    HKL hkl, *layouts;
    int i, count;
    BOOL loaded = FALSE;

    count = GetKeyboardLayoutList(0, NULL);
    layouts = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*layouts));
    if (layouts)
    {
        count = GetKeyboardLayoutList(count, layouts);
        for (i = 0; i < count; i++)
        {
            if (layouts[i] == (HKL)0x04120412)
                loaded = TRUE;
        }
        HeapFree(GetProcessHeap(), 0, layouts);
    }

    hkl = LoadKeyboardLayoutW(L"00000412", KLF_NOTELLSHELL);
    ok(hkl == (HKL)0x04120412, "LoadKeyboardLayoutW returned %p, error %lu\n", hkl, GetLastError());
    if (!hkl)
        return;

    TestHangulHanjaKeys(hkl);

    TestChar(hkl, VK_HANGUL, FALSE, FALSE, L"", 0);
    TestChar(hkl, VK_HANJA, FALSE, FALSE, L"", 0);
    TestChar(hkl, 'Q', FALSE, FALSE, L"q", 1);
    TestChar(hkl, 'Q', TRUE, FALSE, L"Q", 1);
    TestChar(hkl, VK_OEM_102, TRUE, FALSE, L"|", 1);
    TestChar(hkl, VK_BACK, FALSE, FALSE, L"\x0008", 1);
    TestChar(hkl, VK_BACK, FALSE, TRUE, L"\x007f", 1);
    TestChar(hkl, VK_SPACE, FALSE, TRUE, L" ", 1);
    TestChar(hkl, VK_RETURN, FALSE, TRUE, L"\n", 1);
    TestChar(hkl, VK_OEM_4, FALSE, TRUE, L"\x001b", 1);
    TestChar(hkl, '2', TRUE, TRUE, L"\0", 1);
    TestChar(hkl, '6', TRUE, TRUE, L"\x001e", 1);
    TestChar(hkl, VK_OEM_MINUS, TRUE, TRUE, L"\x001f", 1);

    if (!loaded)
        UnloadKeyboardLayout(hkl);
}
