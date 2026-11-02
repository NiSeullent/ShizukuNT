/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests DTD validation of documents with long qualified element names
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>

#define COBJMACROS
#include <initguid.h>
#include <ole2.h>
#include <msxml2.h>

#define LONG_PART_LENGTH 4000

static const WCHAR DtdPrefix[] =
    L"<!DOCTYPE root [<!ELEMENT root (a)><!ELEMENT a EMPTY>]>";

static IXMLDOMDocument2 *CreateDocument(void)
{
    IXMLDOMDocument2 *Document = NULL;
    HRESULT hr;

    hr = CoCreateInstance(&CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER,
                          &IID_IXMLDOMDocument2, (void **)&Document);
    ok(hr == S_OK, "CoCreateInstance returned %#lx\n", hr);
    if (FAILED(hr))
        return NULL;

    hr = IXMLDOMDocument2_put_validateOnParse(Document, VARIANT_FALSE);
    ok(hr == S_OK, "put_validateOnParse returned %#lx\n", hr);
    return Document;
}

static BOOL LoadDocument(IXMLDOMDocument2 *Document, const WCHAR *Xml)
{
    VARIANT_BOOL Success = VARIANT_FALSE;
    BSTR Source;
    HRESULT hr;

    Source = SysAllocString(Xml);
    ok(Source != NULL, "SysAllocString failed\n");
    if (!Source)
        return FALSE;

    hr = IXMLDOMDocument2_loadXML(Document, Source, &Success);
    SysFreeString(Source);
    ok(hr == S_OK, "loadXML returned %#lx\n", hr);
    ok(Success == VARIANT_TRUE, "loadXML reported %d\n", Success);
    return hr == S_OK && Success == VARIANT_TRUE;
}

static WCHAR *BuildLongQNameDocument(void)
{
    static const WCHAR RootOpen[] = L"<root xmlns:";
    static const WCHAR RootNamespace[] = L"=\"urn:libernt-test\"><a/><";
    static const WCHAR RootClose[] = L"/></root>";
    SIZE_T Length;
    WCHAR *Xml, *Cursor;
    SIZE_T i;

    Length = ARRAYSIZE(DtdPrefix) - 1 + ARRAYSIZE(RootOpen) - 1 + LONG_PART_LENGTH +
             ARRAYSIZE(RootNamespace) - 1 + LONG_PART_LENGTH + 1 + LONG_PART_LENGTH +
             ARRAYSIZE(RootClose);
    Xml = HeapAlloc(GetProcessHeap(), 0, Length * sizeof(WCHAR));
    ok(Xml != NULL, "HeapAlloc failed\n");
    if (!Xml)
        return NULL;

    Cursor = Xml;
    memcpy(Cursor, DtdPrefix, sizeof(DtdPrefix) - sizeof(WCHAR));
    Cursor += ARRAYSIZE(DtdPrefix) - 1;
    memcpy(Cursor, RootOpen, sizeof(RootOpen) - sizeof(WCHAR));
    Cursor += ARRAYSIZE(RootOpen) - 1;
    for (i = 0; i < LONG_PART_LENGTH; i++)
        *Cursor++ = L'p';
    memcpy(Cursor, RootNamespace, sizeof(RootNamespace) - sizeof(WCHAR));
    Cursor += ARRAYSIZE(RootNamespace) - 1;
    for (i = 0; i < LONG_PART_LENGTH; i++)
        *Cursor++ = L'p';
    *Cursor++ = L':';
    for (i = 0; i < LONG_PART_LENGTH; i++)
        *Cursor++ = L'n';
    memcpy(Cursor, RootClose, sizeof(RootClose));
    return Xml;
}

static void Test_ValidContent(void)
{
    static const WCHAR Body[] = L"<root><a/></root>";
    WCHAR Xml[ARRAYSIZE(DtdPrefix) + ARRAYSIZE(Body)];
    IXMLDOMDocument2 *Document;
    IXMLDOMParseError *Error = NULL;
    LONG Code = -1;
    HRESULT hr;

    Document = CreateDocument();
    if (!Document)
        return;

    memcpy(Xml, DtdPrefix, sizeof(DtdPrefix) - sizeof(WCHAR));
    memcpy(Xml + ARRAYSIZE(DtdPrefix) - 1, Body, sizeof(Body));
    if (LoadDocument(Document, Xml))
    {
        hr = IXMLDOMDocument2_validate(Document, &Error);
        ok(hr == S_OK, "validate returned %#lx\n", hr);
        ok(Error != NULL, "validate returned no parse error object\n");
        if (Error)
        {
            hr = IXMLDOMParseError_get_errorCode(Error, &Code);
            ok(SUCCEEDED(hr), "get_errorCode returned %#lx\n", hr);
            ok(Code == S_OK, "errorCode is %#lx\n", Code);
            IXMLDOMParseError_Release(Error);
        }
    }

    IXMLDOMDocument2_Release(Document);
}

static void Test_LongQNameContent(void)
{
    IXMLDOMDocument2 *Document;
    IXMLDOMParseError *Error = NULL;
    BSTR Reason = NULL;
    LONG Code = S_OK;
    WCHAR *Xml;
    HRESULT hr;

    Document = CreateDocument();
    if (!Document)
        return;

    Xml = BuildLongQNameDocument();
    if (Xml && LoadDocument(Document, Xml))
    {
        hr = IXMLDOMDocument2_validate(Document, &Error);
        ok(hr == S_FALSE, "validate returned %#lx\n", hr);
        ok(Error != NULL, "validate returned no parse error object\n");
        if (Error)
        {
            hr = IXMLDOMParseError_get_errorCode(Error, &Code);
            ok(hr == S_OK, "get_errorCode returned %#lx\n", hr);
            ok(FAILED(Code), "errorCode is %#lx\n", Code);

            hr = IXMLDOMParseError_get_reason(Error, &Reason);
            ok(hr == S_OK, "get_reason returned %#lx\n", hr);
            ok(Reason != NULL && SysStringLen(Reason) != 0, "reason is empty\n");
            SysFreeString(Reason);
            IXMLDOMParseError_Release(Error);
        }
    }

    if (Xml)
        HeapFree(GetProcessHeap(), 0, Xml);
    IXMLDOMDocument2_Release(Document);
}

START_TEST(DtdValidate)
{
    HRESULT hr;

    hr = CoInitialize(NULL);
    ok(hr == S_OK, "CoInitialize returned %#lx\n", hr);
    if (FAILED(hr))
        return;

    Test_ValidContent();
    Test_LongQNameContent();

    CoUninitialize();
}
