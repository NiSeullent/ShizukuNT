
list(APPEND SOURCE
    NetFrameworkSetup.c
    testlist.c)

add_executable(mscoree_apitest ${SOURCE})
set_module_type(mscoree_apitest win32cui)
add_importlibs(mscoree_apitest advapi32 msvcrt kernel32 ntdll)
add_rostests_file(TARGET mscoree_apitest)
