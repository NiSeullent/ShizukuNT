
include_directories(
    ${REACTOS_SOURCE_DIR}/modules/rostests/apitests/include
    ${REACTOS_SOURCE_DIR}/dll/win32/dnsapi)

list(APPEND SOURCE
    DnsMessage.c
    testlist.c
    ${REACTOS_SOURCE_DIR}/dll/win32/dnsapi/message.c)

add_executable(dnsapi_unittest ${SOURCE})
set_module_type(dnsapi_unittest win32cui)
add_dependencies(dnsapi_unittest psdk)
add_importlibs(dnsapi_unittest msvcrt kernel32 ntdll)
add_rostests_file(TARGET dnsapi_unittest)
