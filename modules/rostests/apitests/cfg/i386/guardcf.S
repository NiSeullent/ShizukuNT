/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     x86 Control Flow Guard load configuration for the CFG test images
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

    .text
    .balign 16
CfgGuardCheckNop:
    ret

    .section .00cfg, "dr"
    .balign 4
    .globl ___guard_check_icall_fptr
___guard_check_icall_fptr:
    .long CfgGuardCheckNop

    .section .rdata, "dr"
    .balign 4
    .globl __load_config_used
__load_config_used:
    .long __load_config_used_end - __load_config_used
    .org __load_config_used + 0x48
    .long ___guard_check_icall_fptr
    .long 0
    .long ___guard_fids_table
    .long ___guard_fids_count
    .long ___guard_flags
    .org __load_config_used + 0x68
    .long ___guard_iat_table
    .long ___guard_iat_count
    .long ___guard_longjmp_table
    .long ___guard_longjmp_count
    .org __load_config_used + 0xa4
    .long ___guard_eh_cont_table
    .long ___guard_eh_cont_count
    .org __load_config_used + 0xc0
__load_config_used_end:
