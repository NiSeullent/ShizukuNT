
add_definitions(
    -DOPT_GENERIC
    -DREAL_IS_FLOAT
    -DNO_CATCHSIGNAL
    -DEOVERFLOW=75
    -Dstrcasecmp=_stricmp
    -Dstrncasecmp=_strnicmp
    -D_DLL -D__USE_CRTIMP
    -D_CRT_NO_POSIX_ERROR_CODES)

include_directories(BEFORE ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/libmpg123)

list(APPEND SOURCE
    src/compat/compat.c
    src/compat/compat_str.c
    src/libmpg123/dct64.c
    src/libmpg123/equalizer.c
    src/libmpg123/feature.c
    src/libmpg123/format.c
    src/libmpg123/frame.c
    src/libmpg123/icy.c
    src/libmpg123/icy2utf8.c
    src/libmpg123/id3.c
    src/libmpg123/index.c
    src/libmpg123/layer1.c
    src/libmpg123/layer2.c
    src/libmpg123/layer3.c
    src/libmpg123/lfs_wrap.c
    src/libmpg123/libmpg123.c
    src/libmpg123/ntom.c
    src/libmpg123/optimize.c
    src/libmpg123/parse.c
    src/libmpg123/readers.c
    src/libmpg123/stringbuf.c
    src/libmpg123/synth.c
    src/libmpg123/synth_8bit.c
    src/libmpg123/synth_real.c
    src/libmpg123/synth_s32.c
    src/libmpg123/tabinit.c)

add_library(libmpg123 ${SOURCE})
target_link_libraries(libmpg123 oldnames)
add_dependencies(libmpg123 psdk)
