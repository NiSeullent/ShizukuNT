/*
 * PROJECT:     LiberNT NT Layer DLL
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     x86 Control Flow Guard check target
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <asm.inc>
#include <ks386.inc>

EXTERN _LdrpValidateUserCallTarget@4:PROC

.code

PUBLIC _LdrpGuardCheckIcall@0
_LdrpGuardCheckIcall@0:
    push eax
    push ecx
    push edx
    sub esp, HEX(80)
    cmp byte ptr ds:[USER_SHARED_DATA + USER_SHARED_DATA_PROCESSOR_FEATURES + PF_XMMI_INSTRUCTIONS_AVAILABLE], 1
    jne LdrpGuardCheckIcallValidate
    movups [esp], xmm0
    movups [esp + HEX(10)], xmm1
    movups [esp + HEX(20)], xmm2
    movups [esp + HEX(30)], xmm3
    movups [esp + HEX(40)], xmm4
    movups [esp + HEX(50)], xmm5
    movups [esp + HEX(60)], xmm6
    movups [esp + HEX(70)], xmm7

LdrpGuardCheckIcallValidate:
    push ecx
    call _LdrpValidateUserCallTarget@4
    test al, al
    jnz LdrpGuardCheckIcallValid
    mov ecx, FAST_FAIL_GUARD_ICALL_CHECK_FAILURE
    int HEX(29)

LdrpGuardCheckIcallValid:
    cmp byte ptr ds:[USER_SHARED_DATA + USER_SHARED_DATA_PROCESSOR_FEATURES + PF_XMMI_INSTRUCTIONS_AVAILABLE], 1
    jne LdrpGuardCheckIcallReturn
    movups xmm0, [esp]
    movups xmm1, [esp + HEX(10)]
    movups xmm2, [esp + HEX(20)]
    movups xmm3, [esp + HEX(30)]
    movups xmm4, [esp + HEX(40)]
    movups xmm5, [esp + HEX(50)]
    movups xmm6, [esp + HEX(60)]
    movups xmm7, [esp + HEX(70)]

LdrpGuardCheckIcallReturn:
    add esp, HEX(80)
    pop edx
    pop ecx
    pop eax
    ret

END
