
include_directories(../include)

list(APPEND KMDFSTUB_DRV_SOURCE
    ../kmtest_drv/kmtest_standalone.c
    KmdfStub_drv.c)

add_library(kmdfstub_drv MODULE ${KMDFSTUB_DRV_SOURCE})
set_module_type(kmdfstub_drv wdmdriver ENTRYPOINT FxDriverEntry 8)
target_link_libraries(kmdfstub_drv kmdfstub kmtest_printf ${PSEH_LIB})
add_importlibs(kmdfstub_drv wdfldr ntoskrnl hal)
target_compile_definitions(kmdfstub_drv PRIVATE KMT_STANDALONE_DRIVER)
add_rostests_file(TARGET kmdfstub_drv)

add_library(kmdfstubnounload_drv MODULE KmdfStubNoUnload_drv.c)
set_module_type(kmdfstubnounload_drv wdmdriver ENTRYPOINT FxDriverEntry 8)
target_link_libraries(kmdfstubnounload_drv kmdfstub)
add_importlibs(kmdfstubnounload_drv wdfldr ntoskrnl hal)
add_rostests_file(TARGET kmdfstubnounload_drv)
