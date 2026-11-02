#
# PROJECT:     LiberNT permissive driver runtime
# LICENSE:     MIT (https://spdx.org/licenses/MIT)
# PURPOSE:     Keep GPL runtime helpers out of modules that are not GPL-compatible
# COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
#

function(set_module_permissive_runtime MODULE)
    if(ARCH STREQUAL "arm64" AND NOT MSVC)
        target_compile_options(${MODULE} PRIVATE "$<$<NOT:$<COMPILE_LANGUAGE:ASM>>:-mno-outline-atomics>")
        set_target_properties(${MODULE} PROPERTIES REACTOS_LSE_ATOMICS "")
    endif()
endfunction()
