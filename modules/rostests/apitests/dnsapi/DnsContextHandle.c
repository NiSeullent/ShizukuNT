/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Tests for DnsAcquireContextHandle and DnsReleaseContextHandle
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <apitest.h>

#include <windns.h>

START_TEST(DnsContextHandle)
{
    HANDLE Context;
    DNS_STATUS Status;

    Context = NULL;
    Status = DnsAcquireContextHandle_W(TRUE, NULL, &Context);
    ok(Status == ERROR_SUCCESS, "DnsAcquireContextHandle_W returned %lu\n", Status);
    ok(Context != NULL, "Context is NULL\n");
    if (Status == ERROR_SUCCESS)
        DnsReleaseContextHandle(Context);

    Context = NULL;
    Status = DnsAcquireContextHandle_A(FALSE, NULL, &Context);
    ok(Status == ERROR_SUCCESS, "DnsAcquireContextHandle_A returned %lu\n", Status);
    ok(Context != NULL, "Context is NULL\n");
    if (Status == ERROR_SUCCESS)
        DnsReleaseContextHandle(Context);
}
