
include_directories(../include)

list(APPEND WDFBIND_DRV_SOURCE
    ../kmtest_drv/kmtest_standalone.c
    WdfBind_drv.c)

add_library(wdfbind_drv MODULE ${WDFBIND_DRV_SOURCE})
set_module_type(wdfbind_drv kmdfdriver KMDF_VERSION 1.33)
target_link_libraries(wdfbind_drv kmtest_printf ${PSEH_LIB})
add_importlibs(wdfbind_drv ntoskrnl hal)
target_compile_definitions(wdfbind_drv PRIVATE KMT_STANDALONE_DRIVER)
add_rostests_file(TARGET wdfbind_drv)
