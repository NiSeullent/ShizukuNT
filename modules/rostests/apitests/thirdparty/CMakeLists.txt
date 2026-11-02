
include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/libpng
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/libtiff
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/zlib
    ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/zlib/contrib)

list(APPEND SOURCE
    libpng.c
    libtiff.c
    minizip.c
    zlib.c
    testlist.c)

add_executable(thirdparty_apitest ${SOURCE})
set_module_type(thirdparty_apitest win32cui)
target_link_libraries(thirdparty_apitest minizip zlib)
add_importlibs(thirdparty_apitest libpng libtiff kernelbase msvcrt kernel32)
add_rostests_file(TARGET thirdparty_apitest)
