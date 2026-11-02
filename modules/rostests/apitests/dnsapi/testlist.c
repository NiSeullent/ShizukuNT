#define STANDALONE
#include <apitest.h>

extern void func_DnsContextHandle(void);
extern void func_DnsQuery(void);

const struct test winetest_testlist[] =
{
    { "DnsContextHandle", func_DnsContextHandle },
    { "DnsQuery", func_DnsQuery },
    { 0, 0 }
};
