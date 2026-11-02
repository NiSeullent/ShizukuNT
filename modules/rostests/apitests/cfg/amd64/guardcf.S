/*
 * PROJECT:     LiberNT API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     AMD64 Control Flow Guard load configuration for the CFG test images
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

    .text
    .balign 16
CfgGuardCheckNop:
    ret

    .balign 16
CfgGuardDispatchNop:
    jmp *%rax

    .section .00cfg, "dr"
    .balign 8
    .globl __guard_check_icall_fptr
__guard_check_icall_fptr:
    .quad CfgGuardCheckNop
    .globl __guard_dispatch_icall_fptr
__guard_dispatch_icall_fptr:
    .quad CfgGuardDispatchNop

    .section .rdata, "dr"
    .balign 8
    .globl _load_config_used
_load_config_used:
    .long _load_config_used_end - _load_config_used
    .org _load_config_used + 0x70
    .quad __guard_check_icall_fptr
    .quad __guard_dispatch_icall_fptr
    .quad __guard_fids_table
    .quad __guard_fids_count
    .long __guard_flags
    .org _load_config_used + 0xa0
    .quad __guard_iat_table
    .quad __guard_iat_count
    .quad __guard_longjmp_table
    .quad __guard_longjmp_count
    .org _load_config_used + 0x108
    .quad __guard_eh_cont_table
    .quad __guard_eh_cont_count
    .org _load_config_used + 0x140
_load_config_used_end:
