#
# PROJECT:     LiberNT permissive driver runtime
# LICENSE:     MIT (https://spdx.org/licenses/MIT)
# PURPOSE:     Build the permissive runtime helpers
# COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
#

include(permissive.cmake)

add_library(kmdfstub STATIC kmdfstub.c)
add_dependencies(kmdfstub bugcodes xdk)
target_include_directories(kmdfstub PUBLIC ${REACTOS_SOURCE_DIR}/sdk/include/wdf/kmdf/1.17)
target_compile_definitions(kmdfstub PUBLIC KMDF_VERSION_MAJOR=1 KMDF_VERSION_MINOR=17)
set_module_permissive_runtime(kmdfstub)
