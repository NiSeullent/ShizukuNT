include_directories(${REACTOS_SOURCE_DIR}/dll/3rdparty/libtirpc/tirpc)

list(APPEND SOURCE
    ClntStat.c
    GetTimeOfDay.c
    SvcRun.c
    testlist.c
    ${REACTOS_SOURCE_DIR}/dll/3rdparty/libtirpc/src/gettimeofday.c)

add_executable(libtirpc_apitest ${SOURCE})
set_module_type(libtirpc_apitest win32cui)
target_link_libraries(libtirpc_apitest ${PSEH_LIB})
add_importlibs(libtirpc_apitest libtirpc ws2_32 msvcrt kernel32 ntdll)
add_rostests_file(TARGET libtirpc_apitest)
