
spec2def(ucrtbase.dll ucrtbase.spec NO_DBG ADD_IMPORTLIB)

# Hack to replace the old CRT include directory with the UCRT include directory
get_property(INCLUDE_DIRS DIRECTORY . PROPERTY INCLUDE_DIRECTORIES)
list(REMOVE_ITEM INCLUDE_DIRS "${REACTOS_SOURCE_DIR}/sdk/include/crt")
set_property(DIRECTORY . PROPERTY INCLUDE_DIRECTORIES ${INCLUDE_DIRS})
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/ucrt)

add_compile_definitions(
    _UCRT
    _CORECRT_BUILD
    CRTDLL
)

list(APPEND UCRTBASE_MUSL_SOURCE
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__cexp.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__cos.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__cosdf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__exp.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__sin.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/__sindf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/carg.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/cargf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/cexp.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/ilogb.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/ilogbf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/lgamma.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/lgamma_r.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/lgammaf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/lgammaf_r.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/nan.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/nanf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/remquo.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/remquof.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/signgam.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/tgamma.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/musl/tgammaf.c
)

if(NOT MSVC)
    set_source_files_properties(${UCRTBASE_MUSL_SOURCE} math.c PROPERTIES COMPILE_OPTIONS "-fno-builtin")
endif()

add_library(ucrtbase SHARED
    printf.c
    stubs.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/acosh.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/acoshf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/asinh.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/asinhf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/atanh.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/atanhf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/cbrt.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/cbrtf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/copysign.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/copysignf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/fdim.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/fmax.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/fmin.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/erf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/erff.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/expm1.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/expm1f.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/log1p.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/log1pf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/nearbyint.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/nearbyintf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/remainder.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/remainderf.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/rint.c
    ${REACTOS_SOURCE_DIR}/sdk/lib/crt/math/rintf.c
    ${UCRTBASE_MUSL_SOURCE}
    math.c
    misc.c
    ucrtbase_stubs.c
    ucrtbase.def
    ucrtbase.rc
)

if(ARCH STREQUAL "amd64")
    target_sources(ucrtbase PRIVATE except1.c amd64/handler4_initializer.c amd64/math_amd64.c)
elseif(ARCH STREQUAL "i386")
    target_sources(ucrtbase PRIVATE except1.c i386/math_i386.c)
else()
    target_sources(ucrtbase PRIVATE fma3_unavailable.c)
endif()

if(ARCH STREQUAL "arm64" AND ARM64EC_RUNTIME)
    target_sources(ucrtbase PRIVATE arm64ec_memory.c)
    set_source_files_properties(arm64ec_memory.c PROPERTIES COMPILE_OPTIONS "-fno-builtin")
elseif(ARCH STREQUAL "arm64")
    set_target_properties(ucrtbase PROPERTIES REACTOS_LSE_ATOMICS lse_atomics_um)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ucrtbase PRIVATE -Wno-builtin-declaration-mismatch)
endif()

set_entrypoint(ucrtbase __acrt_DllMain 12)

target_link_libraries(ucrtbase
    ucrt
    msvcrt_undname_ucrt
    msvcrt_handler4_ucrt
    msvcrt_shared
    crtmath
    vcruntime
    vcstartup
    chkstk
    wine
)

# Implicitly link executable consumers to vcstartup. DLLs provide their own
# entry points and must not receive the executable startup objects.
target_link_libraries(libucrtbase INTERFACE
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:vcstartup>)

if(MSVC)
    target_link_libraries(ucrtbase runtmchk)
else()
    # For __cxa_guard_acquire / __cxa_guard_release
    target_link_libraries(ucrtbase libsupc++)
endif()

add_importlibs(ucrtbase kernel32 ntdll)

add_cd_file(TARGET ucrtbase DESTINATION reactos/system32 FOR all)
